#pragma once

#include <map>
#include <set>
#include <vector>
#include <sparkle/sprout/passes/pass.hpp>

namespace sprk
{
	using ExprHash = uint64_t;

	struct PREResult
	{
		NodeRef original_node;
		NodeRef hoisted_node;
		std::shared_ptr<SproutRegion> target_region;
		int instances_removed = 0;

		std::string to_string() const;
	};

	class PREPass final : public SproutPass
	{
	public:
		PREPass() = default;

		void run(const std::shared_ptr<SproutRegion> &root,
		         std::vector<std::unique_ptr<SproutNode<> > > &nodes) override;

		[[nodiscard]] const std::vector<PREResult> &get_results() const
		{
			return pre_results;
		}

		void dump_results(
			bool colorize = true) const;

	private:
		std::vector<PREResult> pre_results;

		std::map<ExprHash, std::vector<NodeRef> > find_redundant_expressions(
			const std::vector<std::unique_ptr<SproutNode<> > > &nodes);

		ExprHash compute_expr_hash(const SproutNode<> *node) const;

		std::shared_ptr<SproutRegion> find_common_dominator(
			const std::vector<NodeRef> &nodes,
			const std::vector<std::unique_ptr<SproutNode<> > > &node_vec,
			const std::shared_ptr<SproutRegion> &root);

		std::shared_ptr<SproutRegion> find_node_region(
			NodeRef node_ref,
			const std::shared_ptr<SproutRegion> &root);

		std::shared_ptr<SproutRegion> find_common_dominator(
			std::shared_ptr<SproutRegion> r1,
			std::shared_ptr<SproutRegion> r2);
	};
}
