#include <iostream>
#include <sparkle/sprout/passes/dce.hpp>
#include <sparkle/sprout/utils/dump.hpp>

using namespace sprk;

NodeRef make_node(std::vector<std::unique_ptr<SproutNode<>>>& nodes,
				   NodeType type,
				   const std::string& name = "",
				   const std::vector<NodeRef>& inputs = {})
{
	NodeRef id = static_cast<NodeRef>(nodes.size());
	auto node = std::make_unique<SproutNode<>>();

	node->id = id;
	node->type = type;
	node->input_count = 0;
	node->user_count = 0;

	if (!name.empty())
		node->string_id = reinterpret_cast<uint64_t>(strdup(name.c_str()));

	for (NodeRef input : inputs)
	{
		if (input < nodes.size() && nodes[input] && node->input_count < 4)
		{
			node->inputs[node->input_count++] = input;

			if (nodes[input]->user_count < 4)
				nodes[input]->users[nodes[input]->user_count++] = id;
		}
	}

	nodes.push_back(std::move(node));
	return id;
}

void set_const(std::vector<std::unique_ptr<SproutNode<>>>& nodes,
					   NodeRef node_ref,
					   int64_t value)
{
	if (node_ref < nodes.size() && nodes[node_ref])
		nodes[node_ref]->value = value;
}

void create_test_ir(std::vector<std::unique_ptr<SproutNode<>>>& nodes,
                   const std::shared_ptr<SproutRegion>& root_region)
{
    const auto fn_reg = std::make_shared<SproutRegion>("testfn");
    fn_reg->set_type(RegionType::FUNCTION);

    const auto entry_reg = std::make_shared<SproutRegion>("entry");
    entry_reg->set_type(RegionType::BASIC_BLOCK);

    const auto body_reg = std::make_shared<SproutRegion>("body");
    body_reg->set_type(RegionType::BASIC_BLOCK);

    const auto exit_region = std::make_shared<SproutRegion>("exit");
    exit_region->set_type(RegionType::BASIC_BLOCK);

	/*
	 *  $(ROOT)
	 *		-> fn
	 *			-> entry
	 *			-> body
	 *			-> exit
	 */
    root_region->add_child(fn_reg);
    fn_reg->add_child(entry_reg);
    fn_reg->add_child(body_reg);
    fn_reg->add_child(exit_region);

    NodeRef entry = make_node(nodes, NodeType::ENTRY, "entry");
    entry_reg->add_node(entry);
    
    /* fn param */
    NodeRef param_a = make_node(nodes, NodeType::PARAM, "a", { entry });
    NodeRef param_b = make_node(nodes, NodeType::PARAM, "b", { entry });
    entry_reg->add_node(param_a);
    entry_reg->add_node(param_b);
	
    NodeRef const10 = make_node(nodes, NodeType::CONST, "const10");
    set_const(nodes, const10, 10);
    entry_reg->add_node(const10);
    
    NodeRef const20 = make_node(nodes, NodeType::CONST, "const20");
    set_const(nodes, const20, 20);
    entry_reg->add_node(const20);
    
    /* live: a + 10 */
    NodeRef add = make_node(nodes, NodeType::ADD, "a_plus_10", { param_a, const10 });
    body_reg->add_node(add);
    
    /* dead: b + 20 */
    NodeRef mul = make_node(nodes, NodeType::MUL, "b_times_20", { param_b, const20 });
    body_reg->add_node(mul);
    
	/* dead: result + 20 */
    NodeRef dead_add = make_node(nodes, NodeType::ADD, "dead_add", { mul, const10 });
    body_reg->add_node(dead_add);
    
    /* return: use the live add as result */
    NodeRef ret = make_node(nodes, NodeType::RET, "return", { add });
    exit_region->add_node(ret);

    const NodeRef exit = make_node(nodes, NodeType::EXIT, "exit", { ret });
    exit_region->add_node(exit);
}

int main()
{
	std::vector<std::unique_ptr<SproutNode<>>> nodes;
	const auto root = std::make_shared<SproutRegion>("root");
	root->set_type(RegionType::ROOT);

	create_test_ir(nodes, root);

	std::cout << "before DCE optimization: \n";
	dump_ir(root, nodes);

	DCEPass dce;
	dce.run(root, nodes);

	std::cout << "after DCE optimization: \n";
	dce.dump_results(dce.get_dead_nodes(), nodes, root);
}
