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
		for (size_t i = 0; i < nodes.size(); ++i)
		{
			if (!nodes[i] || nodes[i]->type != NodeType::CALL)
				continue;

			if (nodes[i]->input_count > 0) /* find the function being called */
			{
				NodeRef caller_fn = nodes[i]->fn_ref;
				NodeRef callee_fn = nodes[i]->inputs[0];

				if (caller_fn != NULL_REF && callee_fn != NULL_REF)
					ipa_results.call_graph[caller_fn].push_back(callee_fn);
			}
		}
	}

	void IPAPass::analyze_function_purity(const std::vector<std::unique_ptr<SproutNode<> > > &nodes)
	{
		std::unordered_set<NodeRef> potential;

		/* 1st pass: analyze all fn nodes */
		for (size_t i = 0; i < nodes.size(); ++i)
		{
			if (!nodes[i] || nodes[i]->type != NodeType::FUNCTION)
				continue;
			potential.insert(i);
		}

		/* 2nd pass: check impurity */
		for (size_t i = 0; i < nodes.size(); ++i)
		{
			if (!nodes[i])
				continue;

			/* if this is pure */
			if (!is_pure_node(nodes[i].get()))
			{
				NodeRef fn = nodes[i]->fn_ref;
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
				NodeRef constant = nodes[i]->inputs[1];
				if (nodes[constant] && nodes[constant]->type == NodeType::CONST)
				{
					IPAResult::ConstPropOpp opp;
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
			for (NodeRef calle2 : callees)
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
				  [](const IPAResult::InlineOpp& a, const IPAResult::InlineOpp& b)
				  {
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
		auto size = 0;
		for (const auto& node : nodes)
		{
			if (node && node->fn_ref == callee)
				size++;
		}

		/* heuristic cost */
		if (size < 5)
		{
			benefit += 10; /* most preferred */
		}
		else if (size < 20)
		{
			benefit += 5; /* we like small fns */
		}
		
		if (ipa_results.pure_fns.count(callee)) /* if it's pure */
			benefit += 3;

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
			std::cout << "  Inline function #" << callee
					  << " into function #" << caller
					  << " at call site #" << call_site
					  << " (benefit: " << benefit << ")"
					  << (is_recursive ? " [RECURSIVE]" : "")
					  << std::endl;
		}
	}
}
