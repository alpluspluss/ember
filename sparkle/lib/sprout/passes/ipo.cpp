#include <sparkle/sprout/passes/ipo.hpp>
#include <sparkle/sprout/utils/dump.hpp>
#include <sparkle/sprout/utils/irutils.hpp>

namespace sprk
{
	void IPOPass::run(const std::shared_ptr<SproutRegion> &root, std::vector<std::unique_ptr<SproutNode<>>> &nodes)
	{
		ipo_results = {};

		next_node_id = 0;
		for (size_t i = 0; i < nodes.size(); i++)
		{
			if (nodes[i])
				next_node_id = std::max(next_node_id, static_cast<NodeRef>(i + 1));
		}

		prop_constant(nodes);
		perform_inlining(root, nodes);
	}

	void IPOPass::prop_constant(std::vector<std::unique_ptr<SproutNode<> > > &nodes)
	{
		const auto& const_opps = ipa_pass->get_results().const_opps;

        for (const auto& opp : const_opps)
        {
            NodeRef param = opp.param;
            NodeRef const_val = opp.const_val;

            if (param >= nodes.size() || !nodes[param] ||
                const_val >= nodes.size() || !nodes[const_val])
            {
	            continue;
            }

            /* cpy users of the parameter to replace */
            std::vector<NodeRef> users;
            for (uint8_t i = 0; i < nodes[param]->user_count; i++)
                users.push_back(nodes[param]->users[i]);

            for (const NodeRef user : users)
            {
                if (user >= nodes.size() || !nodes[user])
                    continue;

                for (uint8_t i = 0; i < nodes[user]->input_count; i++)
                {
                    if (nodes[user]->inputs[i] == param)
                    {
                        nodes[user]->inputs[i] = const_val;
                        auto already_user = false;
                        for (uint8_t j = 0; j < nodes[const_val]->user_count; j++)
                        {
                            if (nodes[const_val]->users[j] == user)
                            {
                                already_user = true;
                                break;
                            }
                        }

                        if (!already_user && nodes[const_val]->user_count < 4) /* max users */
                            nodes[const_val]->users[nodes[const_val]->user_count++] = user;

                        ipo_results.const_replaced++;
                    }
                }
            }

            /* record */
            ipo_results.const_props.push_back(std::make_pair(param, const_val));
        }
	}

	void IPOPass::perform_inlining(const std::shared_ptr<SproutRegion> &root, std::vector<std::unique_ptr<SproutNode<> > > &nodes)
	{
		const auto& inline_opps = ipa_pass->get_results().inline_opps;

        for (const auto& opp : inline_opps)
        {
            /* skip recursive for now; tailcall later */
            if (opp.is_recursive)
                continue;

            NodeRef caller_fn = opp.caller;
            NodeRef callee_fn = opp.callee;
            NodeRef call_site = opp.call_site;

            if (caller_fn >= nodes.size() || !nodes[caller_fn] ||
                callee_fn >= nodes.size() || !nodes[callee_fn] ||
                call_site >= nodes.size() || !nodes[call_site])
            {
                continue;
            }

            const auto caller_region = find_function_region(caller_fn, root);
            const auto callee_region = find_function_region(callee_fn, root);

            if (!caller_region || !callee_region)
                continue;

            auto call_region = find_node_region(call_site, root);
            if (!call_region)
                continue;

            std::map<NodeRef, NodeRef> param_to_arg;
            map_params_to_args(callee_fn, call_site, nodes, param_to_arg);

            orig_to_clone.clear();

            if (const auto inlined_region = clone_region(callee_region, call_region, nodes);
                !inlined_region)
                continue;

            connect_inlined_nodes(nodes); /* connect the inlined nodes */

            /* replace params with args */
            for (const auto& [param, arg] : param_to_arg)
            {
                if (orig_to_clone.find(param) != orig_to_clone.end())
                {
                    /* cpy users of the cloned parameter */
                    NodeRef cloned_param = orig_to_clone[param];
                    std::vector<NodeRef> users;
                    for (uint8_t i = 0; i < nodes[cloned_param]->user_count; i++)
                        users.push_back(nodes[cloned_param]->users[i]);

                    /* replace all uses of this parameter with the argument */
                    for (NodeRef user : users)
                    {
                        if (user >= nodes.size() || !nodes[user])
                            continue;
                        /* update user input state */
                        for (uint8_t i = 0; i < nodes[user]->input_count; i++)
                        {
                            if (nodes[user]->inputs[i] == cloned_param)
                            {
                                /* update args users */
                                nodes[user]->inputs[i] = arg;
                                if (nodes[arg]->user_count < 4) /* MAX_USER */
                                    nodes[arg]->users[nodes[arg]->user_count++] = user;
                            }
                        }
                    }
                }
            }

            auto returns = collect_function_returns(callee_fn, nodes);
            if (!returns.empty())
            {
                /* note: handle more complex part; use the first return value for now */
                NodeRef original_ret = returns[0];

                if (original_ret < nodes.size() && nodes[original_ret] &&
                    nodes[original_ret]->input_count > 0)
                {
                    /* find cloned ret value */
                    if (NodeRef ret_val = nodes[original_ret]->inputs[0];
                        orig_to_clone.find(ret_val) != orig_to_clone.end())
                    {
                        const NodeRef inlined_ret_val = orig_to_clone[ret_val];
                        replace_call_with_inlined(call_site, inlined_ret_val, nodes);
                        ipo_results.removed_calls++;
                    }
                }
            }

            /* record */
            ipo_results.inlined_functions.push_back(std::make_pair(caller_fn, callee_fn));
        }
	}

    std::shared_ptr<SproutRegion> IPOPass::clone_region(std::shared_ptr<SproutRegion> src_region, std::shared_ptr<SproutRegion> dest_parent, std::vector<std::unique_ptr<SproutNode<> > > &nodes)
    {
	    if (!src_region || !dest_parent)
	    	return nullptr;

	    std::string new_name = src_region->get_name() + "_inlined";
	    auto cloned_region = std::make_shared<SproutRegion>(new_name);

	    cloned_region->set_type(src_region->get_type(), src_region->get_type_id());

	    dest_parent->add_child(cloned_region);
	    for (NodeRef node_ref : src_region->get_nodes())
	    {
	        if (node_ref >= nodes.size() || !nodes[node_ref])
	            continue;

	    	/* skip the boundary IR */
	        if (nodes[node_ref]->type == NodeType::ENTRY ||
                nodes[node_ref]->type == NodeType::EXIT ||
                nodes[node_ref]->type == NodeType::FUNCTION)
	        {
		        continue;
	        }

	        NodeRef cloned_ref = clone_node(node_ref, nodes, next_node_id);
	        if (cloned_ref == NULL_REF)
	        	continue;

	        /* save the map */
	        orig_to_clone[node_ref] = cloned_ref;
	        cloned_region->add_node(cloned_ref);
	    }

	    for (const auto& child : src_region->get_children())
	        clone_region(child, cloned_region, nodes);

	    return cloned_region;
    }

	void IPOPass::connect_inlined_nodes(std::vector<std::unique_ptr<SproutNode<> > > &nodes)
	{
		for (const auto& [orig, clone] : orig_to_clone)
		{
			if (orig >= nodes.size() || !nodes[orig] ||
				clone >= nodes.size() || !nodes[clone])
			{
				continue;
			}

			for (uint8_t i = 0; i < nodes[orig]->input_count; i++)
			{
				NodeRef input = nodes[orig]->inputs[i];
				if (orig_to_clone.find(input) != orig_to_clone.end())
				{
					nodes[clone]->inputs[i] = orig_to_clone[input];
					NodeRef input_clone = orig_to_clone[input];
					if (input_clone < nodes.size() && nodes[input_clone])
					{
						if (nodes[input_clone]->user_count < 4)
							nodes[input_clone]->users[nodes[input_clone]->user_count++] = clone;
					}
				}
				else
				{
					nodes[clone]->inputs[i] = input;
					if (input < nodes.size() && nodes[input])
					{
						if (nodes[input]->user_count < 4)
							nodes[input]->users[nodes[input]->user_count++] = clone;
					}
				}
			}

			nodes[clone]->input_count = nodes[orig]->input_count;
		}
	}

	void IPOPass::map_params_to_args(NodeRef callee, NodeRef call_site, std::vector<std::unique_ptr<SproutNode<> > > &nodes, std::map<NodeRef, NodeRef> &param_to_arg)
	{
		if (callee >= nodes.size() || !nodes[callee] ||
			call_site >= nodes.size() || !nodes[call_site])
		{
			return;
		}

		/* get all params for this fn */
		std::vector<NodeRef> callee_params = collect_function_params(callee, nodes);
		for (uint8_t i = 0; i < nodes[call_site]->input_count; i++)
		{
			const NodeRef input = nodes[call_site]->inputs[i];
			if (input >= nodes.size() || !nodes[input])
				continue;

			/* if this is a call parameter */
			if (nodes[input]->type == NodeType::CALL_PARAM && nodes[input]->input_count >= 2)
			{
				NodeRef index_node = nodes[input]->inputs[0];
				if (index_node >= nodes.size() || !nodes[index_node] ||
					nodes[index_node]->type != NodeType::CONST)
				{
					continue;
				}

				int64_t param_index = 0;
				if (std::holds_alternative<int64_t>(nodes[index_node]->value))
					param_index = std::get<int64_t>(nodes[index_node]->value);

				/* get & map */
				NodeRef arg_value = nodes[input]->inputs[1];
				if (param_index < static_cast<int64_t>(callee_params.size()))
					param_to_arg[callee_params[param_index]] = arg_value;
			}
		}
	}

	void IPOPass::replace_call_with_inlined(NodeRef call_site, NodeRef inlined_return, std::vector<std::unique_ptr<SproutNode<> > > &nodes)
	{
		if (call_site >= nodes.size() || !nodes[call_site] ||
			inlined_return >= nodes.size() || !nodes[inlined_return])
		{
			return;
		}

		/* copy users of the call site */
		std::vector<NodeRef> call_users;
		for (uint8_t i = 0; i < nodes[call_site]->user_count; i++)
			call_users.push_back(nodes[call_site]->users[i]);

		/* replace all uses of the call with the inlined value */
		for (NodeRef user : call_users)
		{
			if (user >= nodes.size() || !nodes[user])
				continue;

			/* update userinput state */
			for (uint8_t i = 0; i < nodes[user]->input_count; i++)
			{
				if (nodes[user]->inputs[i] == call_site)
				{
					nodes[user]->inputs[i] = inlined_return;
					if (nodes[inlined_return]->user_count < 4)
						nodes[inlined_return]->users[nodes[inlined_return]->user_count++] = user;
				}
			}
		}

		/* the call node is now effectively dead; DCE will remove this and all of its input */
	}

	void IPOPass::dump_results(bool colorize)
	{
		const char* blue = colorize ? BLUE : "";
		const char* green = colorize ? GREEN : "";
		const char* reset = colorize ? RESET : "";

		std::cout << blue << "IPO transformation results:" << reset << std::endl;

		std::cout << green << "inlined functions:" << reset << std::endl;
		if (ipo_results.inlined_functions.empty())
		{
			std::cout << "  no functions were inlined." << std::endl;
		}
		else
		{
			for (const auto& [caller, callee] : ipo_results.inlined_functions)
			{
				std::cout << "  function #" << callee << " inlined into #" << caller << std::endl;
			}
			std::cout << "  total calls eliminated: " << ipo_results.removed_calls << std::endl;
		}

		std::cout << green << "constant propagation:" << reset << std::endl;
		if (ipo_results.const_props.empty())
		{
			std::cout << "  no constants were propagated." << std::endl;
		}
		else
		{
			for (const auto& [param, constant] : ipo_results.const_props)
				std::cout << "  parameter #" << param << " replaced with constant #" << constant << std::endl;
			std::cout << "  total replacements: " << ipo_results.const_replaced << std::endl;
		}
	}
}
