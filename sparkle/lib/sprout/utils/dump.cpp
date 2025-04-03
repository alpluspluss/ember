#include <sparkle/sprout/utils/dump.hpp>

namespace sprk
{
	std::string nttostr(NodeType type)
	{
		switch (type)
		{
			case NodeType::ENTRY:
				return "ENTRY";
			case NodeType::EXIT:
				return "EXIT";
			case NodeType::CONST:
				return "CONST";
			case NodeType::PARAM:
				return "PARAM";
			case NodeType::ADD:
				return "ADD";
			case NodeType::SUB:
				return "SUB";
			case NodeType::MUL:
				return "MUL";
			case NodeType::DIV:
				return "DIV";
			case NodeType::CMP:
				return "CMP";
			case NodeType::PHI:
				return "PHI";
			case NodeType::CONTROL:
				return "CONTROL";
			case NodeType::RET:
				return "RET";
			case NodeType::FUNCTION:
				return "FUNCTION";
			case NodeType::CALL:
				return "CALL";
			case NodeType::CALL_PARAM:
				return "CALL_PARAM";
			case NodeType::CALL_RESULT:
				return "CALL_RESULT";
			case NodeType::MALLOC:
				return "MALLOC";
			case NodeType::FREE:
				return "FREE";
			case NodeType::LOAD:
				return "LOAD";
			case NodeType::STORE:
				return "STORE";
			case NodeType::ADDR_OF:
				return "ADDR_OF";
			case NodeType::PTR_LOAD:
				return "PTR_LOAD";
			case NodeType::PTR_STORE:
				return "PTR_STORE";
			case NodeType::PTR_ADD:
				return "PTR_ADD";
			case NodeType::REINTERPRET_CAST:
				return "REINTERPRET_CAST";
			case NodeType::TRUNCATE_CAST:
				return "TRUNCATE_CAST";
			default:
				return "UNKNOWN";
		}
	}

	std::string rttostr(RegionType type)
	{
		switch (type)
		{
			case RegionType::ROOT:
				return "ROOT";
			case RegionType::FUNCTION:
				return "FUNCTION";
			case RegionType::BASIC_BLOCK:
				return "BASIC_BLOCK";
			case RegionType::LOOP_BODY:
				return "LOOP_BODY";
			case RegionType::BRANCH_THEN:
				return "BRANCH_THEN";
			case RegionType::BRANCH_ELSE:
				return "BRANCH_ELSE";
			case RegionType::EXCEPTION:
				return "EXCEPTION";
			default:
				return "UNKNOWN";
		}
	}

	const char *get_color_for_node(const SproutNode<> &node)
	{
		switch (node.type)
		{
			case NodeType::ENTRY:
			case NodeType::EXIT:
				return MAGENTA;
			case NodeType::CONST:
				return CYAN;
			case NodeType::FUNCTION:
			case NodeType::CALL:
			case NodeType::CALL_PARAM:
			case NodeType::CALL_RESULT:
				return GREEN;
			case NodeType::MALLOC:
			case NodeType::FREE:
			case NodeType::LOAD:
			case NodeType::STORE:
			case NodeType::ADDR_OF:
			case NodeType::PTR_LOAD:
			case NodeType::PTR_STORE:
			case NodeType::PTR_ADD:
				return RED;
			default:
				return RESET;
		}
	}

	void dump_region(const std::shared_ptr<SproutRegion> &region,
	                 const std::vector<std::unique_ptr<SproutNode<> > > &nodes, int indent, bool colorize)
	{
		if (!region)
			return;

		std::string indentation(indent * 2, ' ');
		const char *region_color = colorize ? YELLOW : "";
		const char *reset = colorize ? RESET : "";

		std::cout << indentation << region_color << "region: " << region->get_name()
				<< " (" << rttostr(region->get_type());

		if (region->get_type_id() != 0)
			std::cout << ", id: " << region->get_type_id();

		std::cout << ")" << reset << "\n";

		for (NodeRef node_ref: region->get_nodes())
		{
			if (node_ref >= nodes.size() || !nodes[node_ref])
			{
				std::cout << indentation << "  " << (colorize ? RED : "")
						<< "invalid node reference: " << node_ref << reset << "\n";
				continue;
			}

			const auto &node = nodes[node_ref];
			std::stringstream ss;
			ss << format_node(*node, colorize);

			/* split & indent */
			std::string line;
			std::istringstream node_stream(ss.str());
			while (std::getline(node_stream, line))
				std::cout << indentation << "  " << line << "\n";
		}

		for (const auto &child: region->get_children())
			dump_region(child, nodes, indent + 1, colorize);
	}

	void dump_ir(const std::shared_ptr<SproutRegion> &root_region,
	             const std::vector<std::unique_ptr<SproutNode<> > > &nodes, const bool colorize)
	{
		const char *header_color = colorize ? BLUE : "";
		const char *reset = colorize ? RESET : "";

		std::cout << header_color << "Sprout IR dump:" << reset << "\n";
		std::cout << header_color << "-> nodes: " << nodes.size() << reset << "\n";

		dump_region(root_region, nodes, 0, colorize);
	}
}
