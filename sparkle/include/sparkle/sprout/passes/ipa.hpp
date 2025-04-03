#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <set>
#include <queue>
#include <sparkle/sprout/passes/pass.hpp>

namespace sprk
{
	struct IPAResult
	{
		/* constant propagation opportunities */
		struct ConstPropOpp
		{
			NodeRef function;
			NodeRef param;
			NodeRef const_val;
		};

		/* inlining opportinities */
		struct InlineOpp
		{
			NodeRef caller = {};
			NodeRef callee = {};
			NodeRef call_site = {};
			uint16_t benefit = 0;
			bool is_recursive = false; /* for tail call or smth */
		};

		std::unordered_map<NodeRef, std::vector<NodeRef>> call_graph;
		std::unordered_set<NodeRef> pure_fns;
		std::vector<ConstPropOpp> const_opps;
		std::vector<InlineOpp> inline_opps;
	};

	class IPAPass final : public SproutPass
	{
	public:
		IPAPass() = default;

		void run(const std::shared_ptr<SproutRegion>& root,
				 std::vector<std::unique_ptr<SproutNode<>>>& nodes) override;

		[[nodiscard]] const IPAResult& get_results() const
		{
			return ipa_results;
		}

		void dump_results(bool colorize = true);

	private:
		IPAResult ipa_results;

		void build_call_graph(const std::vector<std::unique_ptr<SproutNode<>>>& nodes);

		void analyze_function_purity(const std::vector<std::unique_ptr<SproutNode<>>>& nodes);

		void find_const_prop_opp(const std::vector<std::unique_ptr<SproutNode<>>>& nodes);

		void find_inline_opp(const std::vector<std::unique_ptr<SproutNode<>>>& nodes);

		/* utils */
		bool is_pure_node(const SproutNode<>* node) const;

		[[nodiscard]] uint16_t compute_inlining_benefit(NodeRef callee,
			const std::vector<std::unique_ptr<SproutNode<>>>& nodes) const;
	};
}
