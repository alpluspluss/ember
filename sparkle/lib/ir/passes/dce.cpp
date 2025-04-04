#include <functional>
#include <iostream>
#include <queue>
#include <set>
#include <sparkle/ir/passes/dce.hpp>
#include <sparkle/ir/utils/irdump.hpp>

namespace sprk
{
    void DCEPass::run(const std::shared_ptr<SproutRegion>& root, 
            std::vector<std::unique_ptr<SproutNode<>>>& nodes)
    {
        alive_nodes.clear();
        dead_nodes.clear();

        /* phase 1: mark alive nodes */
        std::queue<NodeRef> worklist;
        for (size_t i = 0; i < nodes.size(); ++i)
        {
            if (!nodes[i])
                continue;

            /* essential flags always live */
            if (const auto& node = nodes[i];
	            node->type == IR_RET || node->type == IR_EXIT ||
                                      (node->props & VOLATILE) == VOLATILE ||
                                      node->type == IR_STORE || node->type == IR_PTR_STORE ||
                                      node->type == IR_FREE)
            {
                worklist.push(static_cast<NodeRef>(i));
                alive_nodes.insert(static_cast<NodeRef>(i));
            }
        }

        /* propagate liveness backend through inputs;
         * if a node isn't connected then it means it is dead */
        while (!worklist.empty())
        {
            const NodeRef curr = worklist.front();
            worklist.pop();
            
            if (curr >= nodes.size() || !nodes[curr]) 
                continue;
        
            for (uint8_t i = 0; i < nodes[curr]->input_count; i++)
            {
                NodeRef input = nodes[curr]->inputs[i];
                if (input >= nodes.size() || !nodes[input]) 
                    continue;
                
                if (alive_nodes.find(input) == alive_nodes.end())
                {
                    alive_nodes.insert(input);
                    worklist.push(input);
                }
            }
        }

        /* phase 2: sweep all the dead nodes */
        for (size_t i = 0; i < nodes.size(); ++i)
        {
            if (!nodes[i])
                continue;

            /* not found */
            if (alive_nodes.find(static_cast<NodeRef>(i)) == alive_nodes.end())
                dead_nodes.insert(static_cast<NodeRef>(i));
        }

        /* statistics; may have macro for optional build later(?) */
        stats.total = 0;
        for (const auto & node : nodes)
        {
            if (node)
                stats.total++;
        }
        stats.dead = dead_nodes.size();
        stats.alive = alive_nodes.size();

        /* phase 3: remove all dead nodes */
        if (dead_nodes.empty())
            return;

        /* for (const NodeRef dead_ref : dead_nodes)
        {
            if (dead_ref >= nodes.size() || !nodes[dead_ref])
                continue;

            for (uint8_t i = 0; i < nodes[dead_ref]->input_count; ++i)
            {
                const NodeRef input_ref = nodes[dead_ref]->inputs[i];
                if (input_ref >= nodes.size() || !nodes[input_ref])
                    continue;

                auto* input_node = nodes[input_ref].get();
                for (uint8_t j = 0; j < input_node->user_count; j++)
                {
                    if (input_node->users[j] == dead_ref)
                    {
                        if (j < input_node->user_count - 1)
                        {
                            input_node->users[j] = NULL_REF;
                        }
                        else
                        {
                            input_node->users[j] = input_node->users[input_node->user_count - 1];
                            input_node->users[input_node->user_count - 1] = NULL_REF;
                        }
                        input_node->user_count--;
                        break;
                    }
                }
            }
        } */

        std::function<void(std::shared_ptr<SproutRegion>&)> clean = [&](std::shared_ptr<SproutRegion>& region)
        {
            if (!region)
                return;
            std::vector<NodeRef> live_nodes_list;
            for (NodeRef node_ref : region->get_nodes())
            {
                if (dead_nodes.find(node_ref) == dead_nodes.end())
                    live_nodes_list.push_back(node_ref);
            }
            region->replace_nodes(live_nodes_list);
            for (auto& child : region->get_children())
                clean(const_cast<std::shared_ptr<SproutRegion>&>(child));
        };

        for (const NodeRef dead_ref : dead_nodes)
        {
            if (dead_ref < nodes.size() && nodes[dead_ref])
                nodes[dead_ref].reset();
        }
        stats.removed = stats.dead;
    }

    void DCEPass::dump_results(const std::set<NodeRef> &dead_nodes,
	                           const std::vector<std::unique_ptr<SproutNode<> > > &nodes,
	                           const std::shared_ptr<SproutRegion> &root,
	                           const bool colorize)
	{
		const char *header_color = colorize ? BLUE : "";
		const char *live_color = colorize ? GREEN : "";
		const char *reset = colorize ? RESET : "";

		std::cout << header_color << "\nDCE results" << reset << "\n";
		std::cout << "found " << dead_nodes.size() << " dead nodes:" << "\n";

		std::cout << "\n";
		std::cout << "remaining nodes: " << (nodes.size() - dead_nodes.size()) << "\n";

		std::function<void(const std::shared_ptr<SproutRegion> &, int)> dump_region =
				[&](const std::shared_ptr<SproutRegion> &region, const int indent)
		{
			if (!region)
				return;

			const std::string indentation(indent * 2, ' ');
			std::cout << indentation << "region: " << region->get_name()
					<< " (" << rttostr(region->get_type()) << ")" << "\n";

			for (NodeRef node_ref: region->get_nodes())
			{
				/* skip the deads */
				if (dead_nodes.find(node_ref) != dead_nodes.end())
					continue;

				if (node_ref >= nodes.size() || !nodes[node_ref])
				{
					std::cout << indentation << "  invalid node reference: " << node_ref << "\n";
					continue;
				}

				const auto &node = nodes[node_ref];
				std::cout << indentation << "  " << live_color << "node #" << node_ref << ": "
						<< nttostr(node->type);

				if (node->str_id)
					std::cout << " (name: " << reinterpret_cast<const char *>(node->str_id) << ")";

				if (std::holds_alternative<int64_t>(node->value))
					std::cout << " = " << std::get<int64_t>(node->value);

				if (node->fn_ref != NULL_REF)
					std::cout << " [fn: " << node->fn_ref << "]";

				if (node->mem_obj != NULL_REF)
					std::cout << " [mem: " << node->mem_obj << "]";

				std::cout << reset << "\n";

				if (node->input_count > 0)
				{
					std::cout << indentation << "    inputs: ";
					auto first = true;
					for (uint8_t i = 0; i < node->input_count; i++)
					{
						if (NodeRef input = node->inputs[i];
							dead_nodes.find(input) == dead_nodes.end())
						{
							if (!first)
								std::cout << ", ";
							std::cout << input;
							first = false;
						}
					}
					std::cout << "\n";
				}

				if (node->user_count > 0)
				{
					std::cout << indentation << "    users: ";
					auto first = true;
					for (uint8_t i = 0; i < node->user_count; i++)
					{
						if (NodeRef user = node->users[i];
							dead_nodes.find(user) == dead_nodes.end())
						{
							if (!first)
								std::cout << ", ";
							std::cout << user;
							first = false;
						}
					}
					std::cout << "\n";
				}
			}

			for (const auto &child: region->get_children())
				dump_region(child, indent + 1);
		};
		dump_region(root, 0);
	}
}
