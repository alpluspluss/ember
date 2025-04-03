#include <sparkle/sprout/utils/irutils.hpp>

namespace sprk
{
	std::shared_ptr<SproutRegion> find_function_region(NodeRef func_node, const std::shared_ptr<SproutRegion> &root)
	{
		if (!root)
			return nullptr;

		if (root->get_type() == RegionType::FUNCTION)
		{
			for (const NodeRef node : root->get_nodes())
			{
				if (node == func_node)
					return root;
			}
		}

		for (const auto& child : root->get_children())
		{
			if (auto result = find_function_region(func_node, child))
				return result;
		}

		return nullptr;
	}

	std::shared_ptr<SproutRegion> find_node_region(NodeRef node_ref, const std::shared_ptr<SproutRegion> &root)
	{
		if (!root)
			return nullptr;

		for (NodeRef n : root->get_nodes())
		{
			if (n == node_ref)
				return root;
		}

		for (const auto& child : root->get_children())
		{
			if (auto result = find_node_region(node_ref, child))
				return result;
		}

		return nullptr;
	}

	std::vector<NodeRef> collect_function_params(NodeRef func_node, const std::vector<std::unique_ptr<SproutNode<> > > &nodes)
	{
		std::vector<NodeRef> params;
		for (size_t i = 0; i < nodes.size(); i++)
		{
			if (!nodes[i])
				continue;

			if (nodes[i]->type == NodeType::PARAM && nodes[i]->fn_ref == func_node)
				params.emplace_back(i);
		}

		return params;
	}

	std::vector<NodeRef> collect_function_returns(NodeRef func_node, const std::vector<std::unique_ptr<SproutNode<> > > &nodes)
	{
		std::vector<NodeRef> returns;
		for (size_t i = 0; i < nodes.size(); i++)
		{
			if (!nodes[i])
				continue;
			if (nodes[i]->type == NodeType::RET && nodes[i]->fn_ref == func_node)
				returns.emplace_back(i);
		}

		return returns;
	}

	NodeRef clone_node(NodeRef orig_node,
							std::vector<std::unique_ptr<SproutNode<>>> &nodes,
							NodeRef &next_id,
							const std::string &suffix)
	{
		if (orig_node >= nodes.size() || !nodes[orig_node])
			return NULL_REF;

		const NodeRef new_id = next_id++;
		if (new_id >= nodes.size())
			nodes.resize(new_id + 1);

		nodes[new_id] = std::make_unique<SproutNode<>>();
		nodes[new_id]->type = nodes[orig_node]->type;
		nodes[new_id]->value = nodes[orig_node]->value;
		nodes[new_id]->id = new_id;

		if (nodes[orig_node]->string_id)
		{
			const std::string orig_name = reinterpret_cast<const char*>(nodes[orig_node]->string_id);
			const std::string new_name = orig_name + suffix;
			auto str_id = new char[new_name.size() + 1];
			std::strcpy(str_id, new_name.c_str());
			nodes[new_id]->string_id = reinterpret_cast<uint64_t>(str_id);
		}

		return new_id;
	}
}