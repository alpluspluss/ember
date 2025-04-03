#include <iostream>
#include <queue>
#include <sparkle/sprout/node.hpp>
#include <sparkle/sprout/region.hpp>
#include <sparkle/sprout/passes/dce.hpp>
#include <sparkle/sprout/utils/dump.hpp>

namespace sprk
{
	void DCEPass::run(const std::shared_ptr<SproutRegion> &root, std::vector<std::unique_ptr<SproutNode<> > > &nodes)
	{
		alive_nodes.clear();
		dead_nodes.clear();

		std::queue<NodeRef> worklist;

		/* 1st pass: find the RET and EXIT nodes */
		for (size_t i = 0; i < nodes.size(); i++)
		{
			if (!nodes[i])
				continue;

			if (nodes[i]->type == NodeType::RET || nodes[i]->type == NodeType::EXIT)
			{
				worklist.push(static_cast<NodeRef>(i));
				alive_nodes.insert(static_cast<NodeRef>(i));
			}
		}

		/* 2nd pass: mark all reachable nodes */
		while (!worklist.empty())
		{
			const NodeRef curr = worklist.front();
			worklist.pop();

			if (curr >= nodes.size() || !nodes[curr])
				continue;

			/* add all inputs to the worklist */
			for (uint8_t i = 0; i < nodes[curr]->input_count; i++)
			{
				NodeRef input = nodes[curr]->inputs[i];

				/* skip invalid */
				if (input >= nodes.size() || !nodes[input])
					continue;

				if (alive_nodes.find(input) == alive_nodes.end())
				{
					alive_nodes.insert(input);
					worklist.push(input);
				}
			}
		}

		/* find & put all unreachable nodes into the dead set */
		for (size_t i = 0; i < nodes.size(); ++i)
		{
			if (nodes[i] && alive_nodes.find(static_cast<NodeRef>(i)) == alive_nodes.end())
				dead_nodes.insert(static_cast<NodeRef>(i));
		}
	}

	void DCEPass::remove_dead_nodes(std::shared_ptr<SproutRegion> &root,
	                                std::vector<std::unique_ptr<SproutNode<> > > &nodes)
	{
		if (dead_nodes.empty())
			return;

		/* disconnect */
		for (const NodeRef dead_ref: dead_nodes)
		{
			if (dead_ref >= nodes.size() || !nodes[dead_ref])
				continue;

			for (uint8_t i = 0; i < nodes[dead_ref]->input_count; i++)
			{
				NodeRef input_ref = nodes[dead_ref]->inputs[i];
				if (input_ref >= nodes.size() || !nodes[input_ref])
					continue;

				auto *input_node = nodes[input_ref].get();
				for (uint8_t j = 0; j < input_node->user_count; j++)
				{
					if (input_node->users[j] == dead_ref)
					{
						/* if it's not the last user, shift the array */
						if (j < input_node->user_count - 1)
						{
							for (uint8_t k = j; k < input_node->user_count - 1; k++)
								input_node->users[k] = input_node->users[k + 1];
						}
						input_node->user_count--;
						break;
					}
				}
			}
		}

		std::function<void(std::shared_ptr<SproutRegion> &)> clean_region =
				[&](const std::shared_ptr<SproutRegion> &region)
		{
			if (!region)
				return;

			std::vector<NodeRef> live_nodes;
			for (NodeRef node_ref: region->get_nodes())
			{
				if (dead_nodes.find(node_ref) == dead_nodes.end())
					live_nodes.push_back(node_ref);
			}


			region->replace_nodes(std::move(live_nodes));
			for (auto child: region->get_children()) /* `get_children()` already returns references */
				clean_region(child);
		};

		clean_region(root);
		for (const NodeRef dead_ref: dead_nodes)
		{
			if (dead_ref < nodes.size() && nodes[dead_ref])
				nodes[dead_ref].reset(); /* free mem */
		}
	}

	void DCEPass::dump_results(const std::set<NodeRef> &dead_nodes,
	                           const std::vector<std::unique_ptr<SproutNode<> > > &nodes,
	                           const std::shared_ptr<SproutRegion> &root,
	                           const bool colorize)
	{
		const char *header_color = colorize ? BLUE : "";
		const char *dead_color = colorize ? RED : "";
		const char *live_color = colorize ? GREEN : "";
		const char *reset = colorize ? RESET : "";

		std::cout << header_color << "DCE results" << reset << "\n";
		std::cout << "found " << dead_nodes.size() << " dead nodes:" << "\n";

		for (const NodeRef node_ref: dead_nodes)
		{
			if (node_ref >= nodes.size() || !nodes[node_ref])
			{
				std::cout << "  " << dead_color << "invalid node reference: "
						<< node_ref << reset << "\n";
				continue;
			}

			const auto &node = nodes[node_ref];
			std::cout << "  " << dead_color << "node #" << node_ref << ": "
					<< nttostr(node->type);

			if (node->string_id)
				std::cout << " (name: " << node->string_id << ")";

			std::cout << reset << "\n";
		}

		std::cout << "\n";
		std::cout << "remaining nodes: " << (nodes.size() - dead_nodes.size()) << "\n";

		std::function<void(const std::shared_ptr<SproutRegion> &, int)> dump_region =
				[&](const std::shared_ptr<SproutRegion> &region, const int indent)
		{
			if (!region)
				return;

			std::string indentation(indent * 2, ' ');
			std::cout << indentation << "region: " << region->get_name()
					<< " (" << rttostr(region->get_type()) << ")" << "\n";

			for (NodeRef node_ref: region->get_nodes())
			{
				/* skip the deads */
				if (dead_nodes.find(node_ref) != dead_nodes.end())
					continue;

				if (node_ref >= nodes.size() || !nodes[node_ref])
				{
					std::cout << indentation << "  invalid node reference: " << node_ref << "\n";
					continue;
				}

				const auto &node = nodes[node_ref];
				std::cout << indentation << "  " << live_color << "node #" << node_ref << ": "
						<< nttostr(node->type);

				if (node->string_id)
					std::cout << " (name: " << reinterpret_cast<const char *>(node->string_id) << ")";

				if (std::holds_alternative<int64_t>(node->value))
					std::cout << " = " << std::get<int64_t>(node->value);

				if (node->fn_ref != NULL_REF)
					std::cout << " [fn: " << node->fn_ref << "]";

				if (node->mem_obj != NULL_REF)
					std::cout << " [mem: " << node->mem_obj << "]";

				std::cout << reset << "\n";

				if (node->input_count > 0)
				{
					std::cout << indentation << "    inputs: ";
					bool first = true;
					for (uint8_t i = 0; i < node->input_count; i++)
					{
						NodeRef input = node->inputs[i];
						if (dead_nodes.find(input) == dead_nodes.end())
						{
							if (!first)
								std::cout << ", ";
							std::cout << input;
							first = false;
						}
					}
					std::cout << "\n";
				}

				if (node->user_count > 0)
				{
					std::cout << indentation << "    users: ";
					auto first = true;
					for (uint8_t i = 0; i < node->user_count; i++)
					{
						if (NodeRef user = node->users[i];
							dead_nodes.find(user) == dead_nodes.end())
						{
							if (!first)
								std::cout << ", ";
							std::cout << user;
							first = false;
						}
					}
					std::cout << "\n";
				}
			}

			for (const auto &child: region->get_children())
				dump_region(child, indent + 1);
		};

		dump_region(root, 0);
	}
}
