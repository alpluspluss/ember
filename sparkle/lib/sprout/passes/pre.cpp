#include <algorithm>
#include <iostream>
#include <sparkle/sprout/passes/pre.hpp>
#include <sparkle/sprout/utils/dump.hpp>

namespace sprk
{
    std::string PREResult::to_string() const
    {
        std::stringstream ss;
        ss << "PRE: Original #" << original_node 
           << " hoisted to #" << hoisted_node 
           << " in region " << target_region->get_name()
           << ". removed " << instances_removed << " redundant instances.";
        return ss.str();
    }
    
    void PREPass::run(const std::shared_ptr<SproutRegion>& root,
                     std::vector<std::unique_ptr<SproutNode<>>>& nodes)
    {
        pre_results.clear();
        auto rdd_exprs = find_redundant_expressions(nodes);
        
        for (const auto& [hash, node_refs] : rdd_exprs)
        {
            if (node_refs.size() <= 1)
                continue;

            /* find common dominator */
            std::shared_ptr<SproutRegion> common_dom = find_common_dominator(node_refs, nodes, root);
            if (!common_dom)
                continue;
                
            /* get the first node to use as a template for the hoisted computation */
            const NodeRef first_node_ref = node_refs[0];
            const SproutNode<>* first_node = first_node_ref < nodes.size() ? nodes[first_node_ref].get() : nullptr;
            if (!first_node)
                continue;

            const auto hoisted_ref = static_cast<NodeRef>(nodes.size());
            auto hoisted_node = std::make_unique<SproutNode<>>();

            hoisted_node->type = first_node->type;
            hoisted_node->id = hoisted_ref;
            if (first_node->string_id)
            {
                std::string orig_name = reinterpret_cast<const char*>(first_node->string_id);
                std::string new_name = orig_name + "_hoisted";
                char* str_id = new char[new_name.size() + 1];
                std::strcpy(str_id, new_name.c_str());
                hoisted_node->string_id = reinterpret_cast<uint64_t>(str_id);
            }

            hoisted_node->fn_ref = first_node->fn_ref;
            for (uint8_t i = 0; i < first_node->input_count && i < 4; i++)
            {
                NodeRef input = first_node->inputs[i];
                if (input < nodes.size() && nodes[input])
                {
                    hoisted_node->inputs[hoisted_node->input_count++] = input;
                    if (nodes[input]->user_count < 4)
                        nodes[input]->users[nodes[input]->user_count++] = hoisted_ref;
                }
            }

            common_dom->add_node(hoisted_ref);
            nodes.push_back(std::move(hoisted_node));

            PREResult result;
            result.original_node = first_node_ref;
            result.hoisted_node = hoisted_ref;
            result.target_region = common_dom;
            for (const NodeRef node_ref : node_refs)
            {
                if (node_ref == hoisted_ref)
                    continue;
                    
                SproutNode<>* node = node_ref < nodes.size() ? nodes[node_ref].get() : nullptr;
                if (!node)
                    continue;

                std::vector<NodeRef> users;
                for (uint8_t i = 0; i < node->user_count; i++)
                    users.push_back(node->users[i]);

                for (NodeRef user : users)
                {
                    if (user >= nodes.size() || !nodes[user])
                        continue;

                    SproutNode<>* user_node = nodes[user].get();
                    for (uint8_t i = 0; i < user_node->input_count; i++)
                    {
                        if (user_node->inputs[i] == node_ref)
                        {
                            user_node->inputs[i] = hoisted_ref;
                            if (nodes[hoisted_ref]->user_count < 4)
                                nodes[hoisted_ref]->users[nodes[hoisted_ref]->user_count++] = user;
                        }
                    }
                }
                
                result.instances_removed++;
            }
            
            pre_results.push_back(result);
        }
    }
    
    std::map<ExprHash, std::vector<NodeRef>> PREPass::find_redundant_expressions(
        const std::vector<std::unique_ptr<SproutNode<>>>& nodes)
    {
        std::map<ExprHash, std::vector<NodeRef>> expressions;
        for (size_t i = 0; i < nodes.size(); i++)
        {
            if (!nodes[i])
                continue;

            if (nodes[i]->type == NodeType::ADD ||
                nodes[i]->type == NodeType::SUB ||
                nodes[i]->type == NodeType::MUL ||
                nodes[i]->type == NodeType::DIV ||
                nodes[i]->type == NodeType::CMP)
            {
                ExprHash hash = compute_expr_hash(nodes[i].get());
                expressions[hash].push_back(i);
            }
        }
        
        /* filter out non-redundant expr */
        std::map<ExprHash, std::vector<NodeRef>> rdd_exprs;
        for (const auto& [hash, node_refs] : expressions)
        {
            if (node_refs.size() > 1)
                rdd_exprs[hash] = node_refs;
        }
        
        return rdd_exprs;
    }
    
    ExprHash PREPass::compute_expr_hash(const SproutNode<>* node) const
    {
        if (!node)
            return 0;

        /* dumn hash algorithm but should work */
        ExprHash hash = static_cast<ExprHash>(node->type);
        if (node->type == NodeType::CONST)
        {
            if (std::holds_alternative<int64_t>(node->value))
                hash = hash * 31 + static_cast<ExprHash>(std::get<int64_t>(node->value));
        }
        
        /* hash the inputs for operations */
        /* sort the inputs first for commutatives */
        if (node->type == NodeType::ADD || node->type == NodeType::MUL)
        {
            std::vector<NodeRef> input_refs;
            for (uint8_t i = 0; i < node->input_count; i++)
                input_refs.push_back(node->inputs[i]);
            std::sort(input_refs.begin(), input_refs.end());

            for (const NodeRef input_ref : input_refs)
                hash = hash * 31 + input_ref;
        }
        else
        {
            /* for non-commutative operations, hash in order */
            for (uint8_t i = 0; i < node->input_count; i++)
                hash = hash * 31 + node->inputs[i];
        }
        
        return hash;
    }
    
    std::shared_ptr<SproutRegion> PREPass::find_common_dominator(
        const std::vector<NodeRef>& nodes,
        const std::vector<std::unique_ptr<SproutNode<>>>& node_vec,
        const std::shared_ptr<SproutRegion>& root)
    {
        if (nodes.empty())
            return nullptr;
            
        /* find the region containing each node */
        std::vector<std::shared_ptr<SproutRegion>> node_regions;
        for (NodeRef node_ref : nodes)
        {
            if (std::shared_ptr<SproutRegion> region = find_node_region(node_ref, root))
                node_regions.push_back(region);
        }
        
        if (node_regions.empty())
            return nullptr;
            
        if (node_regions.size() == 1)
            return node_regions[0];

        std::shared_ptr<SproutRegion> common = node_regions[0];
        for (size_t i = 1; i < node_regions.size(); i++)
            common = find_common_dominator(common, node_regions[i]);
        
        return common;
    }
    
    std::shared_ptr<SproutRegion> PREPass::find_node_region(
        const NodeRef node_ref,
        const std::shared_ptr<SproutRegion>& root)
    {
        if (!root)
            return nullptr;

        for (NodeRef n : root->get_nodes())
        {
            if (n == node_ref)
                return root;
        }

        for (const auto& child : root->get_children())
        {
            if (auto result = find_node_region(node_ref, child))
                return result;
        }
        
        return nullptr;
    }
    
    std::shared_ptr<SproutRegion> PREPass::find_common_dominator(
        std::shared_ptr<SproutRegion> r1,
        std::shared_ptr<SproutRegion> r2)
    {
        if (!r1)
            return r2;
            
        if (!r2)
            return r1;
            
        /* return the dominator if one dominates the other */
        if (r1->dominates(r2))
            return r1;
            
        if (r2->dominates(r1))
            return r2;
            
        /* check the immediate dominator of each otherwise */
        auto idom1 = r1->get_imm_dom();
        auto idom2 = r2->get_imm_dom();
        
        /* then walk up the dominator tree until a common ancestor is found */
        while (r1 && !r1->dominates(r2))
            r1 = r1->get_imm_dom();

        return r1 ? r1 : r2->get_imm_dom();
    }
    
    void PREPass::dump_results(
        const bool colorize) const
    {
        const char* green = colorize ? GREEN : "";
        const char* yellow = colorize ? YELLOW : "";
        const char* reset = colorize ? RESET : "";

        if (pre_results.empty())
        {
            std::cout << "no redundant expressions found for PRE." << std::endl;
        }
        else
        {
            for (const auto& result : pre_results)
                std::cout << green << "  " << result.to_string() << reset << std::endl;
            
            /* print total statistics */
            int total_removed = 0;
            for (const auto& result : pre_results)
                total_removed += result.instances_removed;
            
            std::cout << yellow << "Total redundant computations eliminated: "
                     << total_removed << reset << std::endl;
        }
    }
}