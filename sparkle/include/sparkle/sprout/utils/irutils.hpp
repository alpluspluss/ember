#pragma once

#include <memory>
#include <vector>
#include <sparkle/sprout/node.hpp>
#include <sparkle/sprout/region.hpp>

namespace sprk
{
	std::shared_ptr<SproutRegion> find_function_region(NodeRef func_node,
																const std::shared_ptr<SproutRegion> &root);

	std::shared_ptr<SproutRegion> find_node_region(NodeRef node_ref,
														const std::shared_ptr<SproutRegion> &root);

	std::vector<NodeRef> collect_function_params(NodeRef func_node,
													  const std::vector<std::unique_ptr<SproutNode<>>> &nodes);

	std::vector<NodeRef> collect_function_returns(NodeRef func_node,
													   const std::vector<std::unique_ptr<SproutNode<>>> &nodes);

	NodeRef clone_node(NodeRef orig_node,
							std::vector<std::unique_ptr<SproutNode<>>> &nodes,
							NodeRef &next_id,
							const std::string &suffix = "_inlined");
}
