#include <algorithm>
#include <iostream>
#include <sparkle/sprout/passes/aa.hpp>
#include <sparkle/sprout/utils/dump.hpp>

namespace sprk
{
	std::string MemoryIssue::to_string() const
	{
		switch (type)
		{
			case MemoryIssueType::USE_AFTER_FREE:
				return "<use after free>: memory allocated at node " + std::to_string(memory_obj) +
				       " used at node " + std::to_string(issue_node) +
				       " after being freed at node " + std::to_string(free_node);

			case MemoryIssueType::DOUBLE_FREE:
				return "<double free>: memory allocated at node " + std::to_string(memory_obj) +
				       " freed twice at nodes " + std::to_string(free_node) +
				       " and " + std::to_string(issue_node);

			case MemoryIssueType::MEMORY_LEAK:
				return "<memory leak>: memory allocated at node " + std::to_string(memory_obj) +
				       " never freed";

			case MemoryIssueType::UNINITIALIZED_READ:
				return "<uninitialized read>: memory allocated at node " + std::to_string(memory_obj) +
				       " read at node " + std::to_string(issue_node) +
				       " before being initialized";

			default:
				return "<unknown memory issue>";
		}
	}

	void AliasAnalysisPass::run(const std::shared_ptr<SproutRegion> &root,
	                            std::vector<std::unique_ptr<SproutNode<> > > &nodes)
	{
		memory_operations.clear();
		points_to_map.clear();
		alias_groups.clear();
		issues.clear();

		/* 1st pass: collect all memory operation */
		for (size_t i = 0; i < nodes.size(); i++)
		{
			if (!nodes[i])
				continue;

			/* track alloc node */
			if (nodes[i]->type == NodeType::MALLOC ||
			    nodes[i]->type == NodeType::ADDR_OF)
			{
				memory_operations[i] = {};
			}

			/* track operation on memobj */
			if (nodes[i]->mem_obj != NULL_REF)
				memory_operations[nodes[i]->mem_obj].push_back(i);
		}

		/* pass 2: init p2info */
		for (size_t i = 0; i < nodes.size(); i++)
		{
			if (!nodes[i])
				continue;

			switch (nodes[i]->type)
			{
				case NodeType::ADDR_OF: /* &var -> points to var */
				{
					if (nodes[i]->input_count > 0)
					{
						NodeRef target_var = nodes[i]->inputs[0];
						points_to_map[i].insert(target_var);
					}
					break;
				}

				case NodeType::MALLOC: /* malloc() -> points to itself */
				{
					points_to_map[i].insert(i);
					break;
				}

				default:
					break;
			}
		}

		/* pass 3: propagate p2info */
		/* note: this is a fixed-point algorithm; improve later ig */
		bool changed = true;
		while (changed)
		{
			changed = false;

			for (size_t i = 0; i < nodes.size(); i++)
			{
				if (!nodes[i])
					continue;

				std::set<NodeRef> old_points_to = points_to_map[i];
				switch (nodes[i]->type)
				{
					case NodeType::PHI: /* merge points-to sets of all ins */
					{
						for (uint8_t j = 0; j < nodes[i]->input_count; j++)
						{
							NodeRef input = nodes[i]->inputs[j];
							const auto &input_points_to = points_to_map[input];
							for (NodeRef target: input_points_to)
							{
								if (points_to_map[i].insert(target).second)
									changed = true;
							}
						}
						break;
					}

					case NodeType::PTR_ADD: /* ptr math: p + offset; first in is the pointer */
					{
						if (nodes[i]->input_count > 0)
						{
							NodeRef ptr_input = nodes[i]->inputs[0];
							const auto &input_points_to = points_to_map[ptr_input];

							for (NodeRef target: input_points_to)
							{
								if (points_to_map[i].insert(target).second)
									changed = true;
							}
						}
						break;
					}

					case NodeType::REINTERPRET_CAST: /* ptr casting */
					{
						if (nodes[i]->input_count > 0)
						{
							NodeRef ptr_input = nodes[i]->inputs[0];
							const auto &input_points_to = points_to_map[ptr_input];
							for (NodeRef target: input_points_to)
							{
								if (points_to_map[i].insert(target).second)
									changed = true;
							}
						}
						break;
					}

					default:
						break;
				}
			}
		}

		/* 4th pass: compute alias group */
		std::map<NodeRef, std::set<NodeRef> > target_to_pointers;

		/* invert the points-to relation */
		/* for each target, find all pointers to it */
		for (const auto &[ptr_node, targets]: points_to_map)
		{
			for (NodeRef target: targets)
				target_to_pointers[target].insert(ptr_node);
		}

		/* create an alias group of all pointers per target */
		for (const auto &[target, pointers]: target_to_pointers)
		{
			if (pointers.size() > 1)
				alias_groups.push_back(pointers);
		}

		/* 5th pass: detect all memory issues */
		detect_memory_issues(nodes);
	}

	void AliasAnalysisPass::detect_memory_issues(const std::vector<std::unique_ptr<SproutNode<> > > &nodes)
	{
		for (const auto &[alloc_node, operations]: memory_operations)
		{
			if (operations.empty())
				continue;

			std::vector<NodeRef> sorted_ops = sort_memory_ops(alloc_node, operations, nodes);

			detect_use_after_free(alloc_node, sorted_ops, nodes);
			detect_double_free(alloc_node, sorted_ops, nodes);
			detect_uninitialized_read(alloc_node, sorted_ops, nodes);
		}

		detect_memory_leaks(nodes);
	}

	std::vector<NodeRef> AliasAnalysisPass::sort_memory_ops(NodeRef alloc_node, const std::vector<NodeRef> &operations,
	                                                        const std::vector<std::unique_ptr<SproutNode<> > > &nodes)
	{
		/* currently assumes the IR is roughly in topological order */
		/* TODO: Khan's topo sort algorithm */
		std::vector<NodeRef> result = operations;
		std::sort(result.begin(), result.end());
		return result;
	}

	void AliasAnalysisPass::detect_use_after_free(NodeRef alloc_node,
	                                              const std::vector<NodeRef> &ops,
	                                              const std::vector<std::unique_ptr<SproutNode<> > > &nodes)
	{
		auto is_freed = false;
		NodeRef free_node = NULL_REF;
		for (NodeRef op_node: ops)
		{
			if (op_node >= nodes.size() || !nodes[op_node])
				continue;

			if (nodes[op_node]->type == NodeType::FREE)
			{
				is_freed = true;
				free_node = op_node;
			}
			else if (is_freed &&
			         (nodes[op_node]->type == NodeType::LOAD ||
			          nodes[op_node]->type == NodeType::STORE ||
			          nodes[op_node]->type == NodeType::PTR_LOAD ||
			          nodes[op_node]->type == NodeType::PTR_STORE))
			{
				/* ohemgee! it's use after free! */
				MemoryIssue issue;
				issue.type = MemoryIssueType::USE_AFTER_FREE;
				issue.memory_obj = alloc_node;
				issue.issue_node = op_node;
				issue.free_node = free_node;
				issues.push_back(issue);
			}
		}
	}

	void AliasAnalysisPass::detect_double_free(NodeRef alloc_node, const std::vector<NodeRef> &ops,
	                                           const std::vector<std::unique_ptr<SproutNode<> > > &nodes)
	{
		auto is_freed = false;
		NodeRef first_free_node = NULL_REF;

		for (const NodeRef op_node: ops)
		{
			if (op_node >= nodes.size() || !nodes[op_node])
				continue;

			if (nodes[op_node]->type == NodeType::FREE)
			{
				if (is_freed)
				{
					/* double free */
					MemoryIssue issue;
					issue.type = MemoryIssueType::DOUBLE_FREE;
					issue.memory_obj = alloc_node;
					issue.issue_node = op_node;
					issue.free_node = first_free_node;
					issues.push_back(issue);
				}
				else
				{
					is_freed = true;
					first_free_node = op_node;
				}
			}
		}
	}

	void AliasAnalysisPass::detect_uninitialized_read(NodeRef alloc_node, const std::vector<NodeRef> &ops,
	                                                  const std::vector<std::unique_ptr<SproutNode<> > > &nodes)
	{
		auto is_initialized = false;
		for (NodeRef op_node: ops)
		{
			if (op_node >= nodes.size() || !nodes[op_node])
				continue;

			if (nodes[op_node]->type == NodeType::STORE ||
			    nodes[op_node]->type == NodeType::PTR_STORE)
			{
				is_initialized = true;
			}
			else if (!is_initialized &&
			         (nodes[op_node]->type == NodeType::LOAD ||
			          nodes[op_node]->type == NodeType::PTR_LOAD))
			{
				/* detected!!!!!!!!!! :< */
				MemoryIssue issue;
				issue.type = MemoryIssueType::UNINITIALIZED_READ;
				issue.memory_obj = alloc_node;
				issue.issue_node = op_node;
				issue.free_node = NULL_REF;
				issues.push_back(issue);
			}
		}
	}

	void AliasAnalysisPass::detect_memory_leaks(const std::vector<std::unique_ptr<SproutNode<> > > &nodes)
	{
		for (const auto &[alloc_node, operations]: memory_operations)
		{
			if (nodes[alloc_node]->type != NodeType::MALLOC)
				continue;

			auto is_freed = false;
			for (const NodeRef op_node: operations)
			{
				if (op_node >= nodes.size() || !nodes[op_node])
					continue;

				if (nodes[op_node]->type == NodeType::FREE)
				{
					is_freed = true;
					break;
				}
			}

			if (!is_freed)
			{
				/* possible leak */
				MemoryIssue issue;
				issue.type = MemoryIssueType::MEMORY_LEAK;
				issue.memory_obj = alloc_node;
				issue.issue_node = NULL_REF;
				issue.free_node = NULL_REF;
				issues.push_back(issue);
			}
		}
	}

	bool AliasAnalysisPass::points_to_same_memory(const NodeRef ptr1, const NodeRef ptr2) const
	{
		const auto it1 = points_to_map.find(ptr1);
		const auto it2 = points_to_map.find(ptr2);
		if (it1 == points_to_map.end() || it2 == points_to_map.end())
			return true;

		/* if intersects */
		const auto &set1 = it1->second;
		const auto &set2 = it2->second;
		for (NodeRef target: set1)
		{
			if (set2.find(target) != set2.end())
				return true;
		}

		return false;
	}

	const std::set<NodeRef> &AliasAnalysisPass::get_points_to_set(const NodeRef ptr) const
	{
		static const std::set<NodeRef> empty_set;
		const auto it = points_to_map.find(ptr);
		return (it != points_to_map.end()) ? it->second : empty_set;
	}

	void AliasAnalysisPass::dump_results(const std::vector<std::unique_ptr<SproutNode<> > > &nodes,
	                                     const std::shared_ptr<SproutRegion> &root, bool colorize) const
	{
		const char *red = colorize ? RED : "";
		const char *yellow = colorize ? YELLOW : "";
		const char *blue = colorize ? BLUE : "";
		const char *cyan = colorize ? CYAN : "";
		const char *reset = colorize ? RESET : "";

		std::cout << blue << "memory issues:" << reset << std::endl;
		if (issues.empty())
		{
			std::cout << "  no memory issues detected." << std::endl;
		}
		else
		{
			for (const auto &issue: issues)
				std::cout << red << "  " << issue.to_string() << reset << std::endl;
		}

		std::cout << blue << "\nalias groups:" << reset << std::endl;
		if (alias_groups.empty())
		{
			std::cout << "  no alias groups found." << std::endl;
		}
		else
		{
			for (size_t i = 0; i < alias_groups.size(); i++)
			{
				const auto &group = alias_groups[i];
				std::cout << yellow << "  group " << i << ":" << reset << " ";

				auto first = true;
				for (const NodeRef ptr: group)
				{
					if (!first)
						std::cout << ", ";
					first = false;

					std::cout << ptr;

					if (ptr < nodes.size() && nodes[ptr])
					{
						const auto &node = nodes[ptr];
						if (node->string_id)
							std::cout << " (" << reinterpret_cast<const char *>(node->string_id) << ")";
					}
				}
				std::cout << std::endl;
			}
		}

		std::cout << blue << "\npoints-to information:" << reset << std::endl;
		if (points_to_map.empty())
		{
			std::cout << "  no points-to information found." << std::endl;
		}
		else
		{
			for (const auto &[ptr, targets]: points_to_map)
			{
				if (targets.empty())
					continue;

				std::cout << cyan << "  node #" << ptr;

				if (ptr < nodes.size() && nodes[ptr])
				{
					const auto &node = nodes[ptr];
					std::cout << " (" << nttostr(node->type);
					if (node->string_id)
						std::cout << " " << reinterpret_cast<const char *>(node->string_id);
					std::cout << ")";
				}

				std::cout << reset << " points to: ";

				auto first = true;
				for (const NodeRef target: targets)
				{
					if (!first)
						std::cout << ", ";
					first = false;

					std::cout << target;

					if (target < nodes.size() && nodes[target])
					{
						if (const auto &node = nodes[target];
							node->string_id)
							std::cout << " (" << reinterpret_cast<const char *>(node->string_id) << ")";
					}
				}
				std::cout << std::endl;
			}
		}
	}
}
