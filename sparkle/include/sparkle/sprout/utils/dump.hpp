#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <sparkle/sprout/node.hpp>
#include <sparkle/sprout/region.hpp>

namespace sprk
{
	constexpr auto RESET = "\033[0m";
	constexpr auto RED = "\033[31m";
	constexpr auto GREEN = "\033[32m";
	constexpr auto YELLOW = "\033[33m";
	constexpr auto BLUE = "\033[34m";
	constexpr auto MAGENTA = "\033[35m";
	constexpr auto CYAN = "\033[36m";

	std::string nttostr(NodeType type);

	std::string rttostr(RegionType type);

	const char *get_color_for_node(const SproutNode<> &node);

	template<size_t MAX_INPUTS = 4, size_t MAX_USERS = 4>
	std::string format_node(const SproutNode<MAX_INPUTS, MAX_USERS> &node, bool colorize = true)
	{
		std::stringstream ss;

		const char *color = colorize ? get_color_for_node(node) : "";
		const char *reset = colorize ? RESET : "";
		ss << color << "node #" << node.id << ": " << nttostr(node.type);

		if (node.string_id)
			ss << " (name: " << node.string_id << ")";

		if (node.type == NodeType::CONST)
		{
			if (std::holds_alternative<int64_t>(node.value))
				ss << " = " << std::get<int64_t>(node.value);
			else if (std::holds_alternative<double>(node.value))
				ss << " = " << std::get<double>(node.value);
			else if (std::holds_alternative<std::string>(node.value))
				ss << " = \"" << std::get<std::string>(node.value) << "\"";
		}

		if (node.fn_ref != NULL_REF)
			ss << " [fn: " << node.fn_ref << "]";

		if (node.mem_obj != NULL_REF)
			ss << " [mem: " << node.mem_obj << "]";

		if (node.input_count > 0)
		{
			ss << "\n  inputs: ";
			for (uint8_t i = 0; i < node.input_count; ++i)
			{
				if (i > 0)
					ss << ", ";
				ss << node.inputs[i];
			}
		}

		if (node.user_count > 0)
		{
			ss << "\n  users: ";
			for (uint8_t i = 0; i < node.user_count; ++i)
			{
				if (i > 0)
					ss << ", ";
				ss << node.users[i];
			}
		}

		ss << reset;
		return ss.str();
	}

	template<size_t MAX_INPUTS = 4, size_t MAX_USERS = 4>
	void dump_node(const SproutNode<MAX_INPUTS, MAX_USERS> &node, bool colorize = true)
	{
		std::cout << format_node(node, colorize) << "\n";
	}

	void dump_region(const std::shared_ptr<SproutRegion> &region,
	                 const std::vector<std::unique_ptr<SproutNode<> > > &nodes,
	                 int indent = 0,
	                 bool colorize = true);

	void dump_ir(
		const std::shared_ptr<SproutRegion> &root_region,
		const std::vector<std::unique_ptr<SproutNode<> > > &nodes,
		bool colorize = true
	);
}
