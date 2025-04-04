#pragma once

#include <map>
#include <set>
#include <vector>
#include <sparkle/ir/passes/pass.hpp>

namespace sprk
{
	enum class MemoryIssueType : uint8_t
	{
		USE_AFTER_FREE,
		DOUBLE_FREE,
		MEMORY_LEAK,
		UNINITIALIZED_READ
	};

	struct PointsToInfo
	{
		NodeRef alloc_node = {};

		/* coalesable to uint8_t flags = ...; */
		bool is_init = false;
		bool is_free = false;
	};

	struct MemoryIssue
	{
		NodeRef memory_obj;
		NodeRef issue_node;
		NodeRef free_node;
		MemoryIssueType type;

		[[nodiscard]] std::string to_string() const;
	};

	class AliasAnalysisPass final : public SproutPass
	{
	public:
		AliasAnalysisPass() = default;

		void run(const std::shared_ptr<SproutRegion> &root,
		         std::vector<std::unique_ptr<SproutNode<> > > &nodes) override;

		[[nodiscard]] const std::vector<MemoryIssue> &get_issues() const
		{
			return issues;
		}

		[[nodiscard]] bool points_to_same_memory(NodeRef ptr1, NodeRef ptr2) const;

		[[nodiscard]] const std::set<NodeRef> &get_points_to_set(NodeRef ptr) const;

		void dump_results(const std::vector<std::unique_ptr<SproutNode<>>> &nodes,
		                  bool colorize = true) const;

	private:
		std::map<NodeRef, std::vector<NodeRef> > memory_operations; /* alloc node -> operations */
		std::map<NodeRef, std::set<NodeRef> > points_to_map;        /* ptr -> set of memory objects */
		std::vector<std::set<NodeRef> > alias_groups;               /* groups of pointers that may alias */
		std::vector<MemoryIssue> issues;                            /* detected issues */

		void detect_memory_issues(const std::vector<std::unique_ptr<SproutNode<> > > &nodes);

		void detect_use_after_free(NodeRef alloc_node, const std::vector<NodeRef> &ops,
		                           const std::vector<std::unique_ptr<SproutNode<> > > &nodes);

		void detect_double_free(NodeRef alloc_node, const std::vector<NodeRef> &ops,
		                        const std::vector<std::unique_ptr<SproutNode<> > > &nodes);

		void detect_uninitialized_read(NodeRef alloc_node, const std::vector<NodeRef> &ops,
		                               const std::vector<std::unique_ptr<SproutNode<> > > &nodes);

		void detect_memory_leaks(const std::vector<std::unique_ptr<SproutNode<> > > &nodes);

		static std::vector<NodeRef> sort_memory_ops(
			const std::vector<NodeRef> &operations);
	};
}
