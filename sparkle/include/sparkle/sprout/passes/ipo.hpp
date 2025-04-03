#pragma once

#include <map>
#include <vector>
#include <functional>
#include <sparkle/sprout/passes/pass.hpp>
#include <sparkle/sprout/passes/ipa.hpp>

namespace sprk
{
	struct IPOResult
	{
		std::vector<std::pair<NodeRef, NodeRef> > inlined_functions; /* caller-callee pairs */
		std::vector<std::pair<NodeRef, NodeRef> > const_props;       /* param constants pairs */

		uint32_t removed_calls = 0;
		uint32_t const_replaced = 0;
	};

	class IPOPass final : public SproutPass
	{
	public:
		explicit IPOPass(const std::shared_ptr<IPAPass> &ipa_pass) : ipa_pass(ipa_pass) {}

		void run(const std::shared_ptr<SproutRegion> &root,
		         std::vector<std::unique_ptr<SproutNode<> > > &nodes) override;

		[[nodiscard]] const IPOResult &get_results() const
		{
			return ipo_results;
		}

		void dump_results(bool colorize = true);

	private:
		IPOResult ipo_results;
		std::map<NodeRef, NodeRef> orig_to_clone;
		std::set<NodeRef> functions_to_remove;
		std::unordered_set<NodeRef> nodes_to_remove;
		std::shared_ptr<IPAPass> ipa_pass;
		NodeRef next_node_id = {}; /* here to track next node id for cloning */

		void perform_inlining(const std::shared_ptr<SproutRegion> &root,
		                      std::vector<std::unique_ptr<SproutNode<> > > &nodes);

		/* propagate constant */
		void prop_constant(const std::vector<std::unique_ptr<SproutNode<> > > &nodes);

		std::shared_ptr<SproutRegion> clone_region(const std::shared_ptr<SproutRegion> &src_region,
		                                           const std::shared_ptr<SproutRegion> &dest_parent,
		                                           std::vector<std::unique_ptr<SproutNode<> > > &nodes);

		void connect_inlined_nodes(std::vector<std::unique_ptr<SproutNode<> > > &nodes);

		static void map_params_to_args(NodeRef callee,
		                        NodeRef call_site,
		                        const std::vector<std::unique_ptr<SproutNode<> > > &nodes,
		                        std::map<NodeRef, NodeRef> &param_to_arg);

		void replace_call_with_inlined(NodeRef call_site,
		                               NodeRef inlined_return,
		                               std::vector<std::unique_ptr<SproutNode<> > > &nodes);

		void remove_dead_functions(const std::shared_ptr<SproutRegion> &root,
		                           std::vector<std::unique_ptr<SproutNode<> > > &nodes);

		std::vector<NodeRef> inline_function_body(
			const std::shared_ptr<SproutRegion> &callee_region,
			const std::shared_ptr<SproutRegion> &caller_region,
			std::vector<std::unique_ptr<SproutNode<> > > &nodes);
	};
}
