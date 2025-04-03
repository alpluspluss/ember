#include <algorithm>
#include <iostream>
#include <sparkle/sprout/passes/ipa.hpp>
#include <sparkle/sprout/utils/dump.hpp>

namespace sprk
{
	void IPAPass::run(const std::shared_ptr<SproutRegion> &root, std::vector<std::unique_ptr<SproutNode<> > > &nodes)
	{
		ipa_results = {}; /* clear */

		build_call_graph(nodes);
		analyze_function_purity(nodes);
		find_const_prop_opp(nodes);
		find_inline_opp(nodes);
	}

	void IPAPass::build_call_graph(const std::vector<std::unique_ptr<SproutNode<>>>& nodes)
	{
		for (const auto& node : nodes)
		{
			if (!node || node->type != NodeType::CALL)
				continue;

			if (node->input_count > 0) /* find the function being called */
			{
				NodeRef caller_fn = node->fn_ref;
				NodeRef callee_fn = node->inputs[0];

				if (caller_fn != NULL_REF && callee_fn != NULL_REF)
					ipa_results.call_graph[caller_fn].push_back(callee_fn);
			}
		}
	}

	void IPAPass::analyze_function_purity(const std::vector<std::unique_ptr<SproutNode<> > > &nodes)
	{
		/* 1st pass: analyze all fn nodes */
		std::unordered_set<NodeRef> potential;
		for (size_t i = 0; i < nodes.size(); ++i)
		{
			if (!nodes[i] || nodes[i]->type != NodeType::FUNCTION)
				continue;
			potential.insert(i);
		}

		/* 2nd pass: check impurity */
		for (const auto& node : nodes)
		{
			if (!node)
				continue;

			/* if this is pure */
			if (!is_pure_node(node.get()))
			{
				NodeRef fn = node->fn_ref;
				potential.erase(fn);
			}
		}

		/* update state */
		ipa_results.pure_fns = potential;
	}

	void IPAPass::find_const_prop_opp(const std::vector<std::unique_ptr<SproutNode<> > > &nodes)
	{
		for (size_t i = 0; i < nodes.size(); ++i)
		{
			if (!nodes[i] || nodes[i]->type != NodeType::CALL_PARAM)
				continue;

			/* if it is a literal/constant */
			if (nodes[i]->input_count == 2)
			{
				if (NodeRef constant = nodes[i]->inputs[1];
					nodes[constant] && nodes[constant]->type == NodeType::CONST)
				{
					IPAResult::ConstPropOpp opp = {};
					opp.function = nodes[i]->fn_ref;
					opp.param = i;
					opp.const_val = constant;
					ipa_results.const_opps.push_back(opp);
				}
			}
		}
	}

	void IPAPass::find_inline_opp(const std::vector<std::unique_ptr<SproutNode<> > > &nodes)
	{
		for (const auto& [caller, callees] : ipa_results.call_graph)
		{
			for (const NodeRef calle2 : callees)
			{
				const bool is_recursive = (caller == calle2);
				for (size_t i = 0; i < nodes.size(); ++i)
				{
					/* find call sites */
					if (!nodes[i] || nodes[i]->type != NodeType::CALL)
						continue;

					if (nodes[i]->inputs[0] == calle2)
					{
						IPAResult::InlineOpp opp;
						opp.caller = caller;
						opp.callee = calle2;
						opp.call_site = i;
						opp.is_recursive = is_recursive;
						opp.benefit = compute_inlining_benefit(calle2, nodes);

						ipa_results.inline_opps.push_back(opp);
					}
				}
			}
		}

		std::sort(ipa_results.inline_opps.begin(),
			  ipa_results.inline_opps.end(),
			  [&nodes](const IPAResult::InlineOpp& a, const IPAResult::InlineOpp& b)
			  {
				  const auto size_a = std::count_if(nodes.begin(), nodes.end(),
					  [&](const auto& node)
					  {
						  return node && node->fn_ref == a.callee;
					  });
				  const auto size_b = std::count_if(nodes.begin(), nodes.end(),
					  [&](const auto& node)
					  {
						  return node && node->fn_ref == b.callee;
					  });

				  if (size_a != size_b)
					  return size_a > size_b;

			  	  /* benefit-based if the sizes are the same */
				  return a.benefit > b.benefit;
			  });
	}

	bool IPAPass::is_pure_node(const SproutNode<> *node) const
	{
		/* note: maybe make this constexpr */
		static const std::unordered_set pure_types = {
			NodeType::CONST,
			NodeType::PARAM,
			NodeType::ADD,
			NodeType::SUB,
			NodeType::MUL,
			NodeType::DIV,
			NodeType::CMP
		};

		return pure_types.count(node->type) > 0;
	}

	uint16_t IPAPass::compute_inlining_benefit(const NodeRef callee, const std::vector<std::unique_ptr<SproutNode<> > > &nodes) const
	{
		auto benefit = 0;
		auto size = std::count_if(nodes.begin(), nodes.end(),
			[callee](const auto& node)
			{
				return node && node->fn_ref == callee;
			});

		if (size > 50)
			return 0;

		/*
		 * call graph dependency based inlining selection
		 * 1. inline the leaf functions
		 *	 that don't call other functions are great for
		 *	 inlining since they won't require further inlining
		 *
		 * 2. functions that are called by many other functions
		 *	 solely because we can do more DCE and other optimizations later
		 *
		 * 3. functions that call other functions
		 *   so inlining a function that itself contains calls is possible
		 */

		const bool is_leaf = (ipa_results.call_graph.find(callee) == ipa_results.call_graph.end() ||
				   ipa_results.call_graph.at(callee).empty());
		if (is_leaf)
			benefit += 10;

		auto num_callers = 0;
		for (const auto& [caller, callees] : ipa_results.call_graph)
		{
			for (NodeRef called : callees)
			{
				if (called == callee)
					num_callers++;
			}
		}
		benefit += std::min(num_callers * 2, 10); /* cap at +10 */

		if (ipa_results.call_graph.find(callee) != ipa_results.call_graph.end())
		{
			const size_t num_callees = ipa_results.call_graph.at(callee).size();
			benefit += std::min(num_callees * 3, 15UL); /* cap at + 15 */
		}

		/* NOTE: more factors down here */
		if (ipa_results.pure_fns.count(callee))
			benefit += 5;

		return benefit;
	}

	void IPAPass::dump_results(bool colorize)
	{
		const char* blue = colorize ? BLUE : "";
		const char* reset = colorize ? RESET : "";

		/* dump call graph */
		std::cout << blue << "call graph:" << reset << std::endl;
		for (const auto& [caller, callees] : ipa_results.call_graph)
		{
			std::cout << "  caller #" << caller << " calls: ";
			for (const NodeRef callee : callees)
				std::cout << callee << " ";
			std::cout << std::endl;
		}

		/* dump pure functions */
		std::cout << blue << "\npure fns:" << reset << std::endl;
		for (NodeRef pure_fn : ipa_results.pure_fns)
			std::cout << "  function #" << pure_fn << std::endl;

		/* dump constant propagation */
		std::cout << blue << "\nconstant propagation opportunities:" << reset << std::endl;
		for (const auto&[function, param, const_val] : ipa_results.const_opps)
		{
			std::cout << "  function #" << function
					  << " param #" << param
					  << " constant #" << const_val << std::endl;
		}

		/* inlining opp */
		std::cout << blue << "\ninlining opportunities:" << reset << std::endl;
		for (const auto&[caller, callee, call_site, benefit, is_recursive] : ipa_results.inline_opps)
		{
			std::cout << "  inline function #" << callee
					  << " into function #" << caller
					  << " at call site #" << call_site
					  << " (benefit: " << benefit << ")"
					  << (is_recursive ? " [RECURSIVE]" : "")
					  << std::endl;
		}
	}
}
