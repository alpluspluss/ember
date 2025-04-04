#include <iostream>
#include <sparkle/ir/passes/dce.hpp>
#include <sparkle/ir/utils/irdump.hpp>

using namespace sprk;

NodeRef make_node(std::vector<std::unique_ptr<SproutNode<> > > &nodes,
                  const NodeType type,
                  const std::string &name = "",
                  const std::vector<NodeRef> &inputs = {})
{
	const auto id = static_cast<NodeRef>(nodes.size());
	auto node = std::make_unique<SproutNode<> >();

	node->id = id;
	node->type = type;
	node->input_count = 0;
	node->user_count = 0;

	if (!name.empty())
		node->str_id = reinterpret_cast<uint64_t>(strdup(name.c_str()));

	for (const NodeRef input: inputs)
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

void set_const(const std::vector<std::unique_ptr<SproutNode<> > > &nodes,
               NodeRef node_ref,
               int64_t value)
{
	if (node_ref < nodes.size() && nodes[node_ref])
		nodes[node_ref]->value = value;
}

void create_test_ir(std::vector<std::unique_ptr<SproutNode<> > > &nodes,
                    const std::shared_ptr<SproutRegion> &root_region)
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

	NodeRef const10 = make_node(nodes, NodeType::LIT, "const10");
	set_const(nodes, const10, 10);
	entry_reg->add_node(const10);

	NodeRef const20 = make_node(nodes, NodeType::LIT, "const20");
	set_const(nodes, const20, 20);
	entry_reg->add_node(const20);

	/* live: a + 10 */
	NodeRef add = make_node(nodes, NodeType::ADD, "a_plus_10", { param_a, const10 });
	body_reg->add_node(add);

	/* dead: b + 20 */
	NodeRef mul = make_node(nodes, NodeType::MUL, "b_times_20", { param_b, const20 });
	body_reg->add_node(mul);

	/* dead: result + 20 */
	const NodeRef dead_add = make_node(nodes, NodeType::ADD, "dead_add", { mul, const10 });
	body_reg->add_node(dead_add);

	/* return: use the live add as result */
	NodeRef ret = make_node(nodes, NodeType::RET, "return", { add });
	exit_region->add_node(ret);

	const NodeRef exit = make_node(nodes, NodeType::EXIT, "exit", { ret });
	exit_region->add_node(exit);
}

void create_br_ir(std::vector<std::unique_ptr<SproutNode<> > > &nodes,
                              const std::shared_ptr<SproutRegion> &root_region)
{
	const auto fn_reg = std::make_shared<SproutRegion>("brfn");
	fn_reg->set_type(RegionType::FUNCTION);

	const auto entry_reg = std::make_shared<SproutRegion>("entry");
	entry_reg->set_type(RegionType::BASIC_BLOCK);

	const auto cond_reg = std::make_shared<SproutRegion>("condition");
	cond_reg->set_type(RegionType::BASIC_BLOCK);

	const auto then_reg = std::make_shared<SproutRegion>("then_br");
	then_reg->set_type(RegionType::BR_THEN);

	const auto else_reg = std::make_shared<SproutRegion>("else_br");
	else_reg->set_type(RegionType::BR_ELSE);

	const auto exit_reg = std::make_shared<SproutRegion>("exit");
	exit_reg->set_type(RegionType::BASIC_BLOCK);

	root_region->add_child(fn_reg);
	fn_reg->add_child(entry_reg);
	fn_reg->add_child(cond_reg);
	fn_reg->add_child(then_reg);
	fn_reg->add_child(else_reg);
	fn_reg->add_child(exit_reg);

	NodeRef entry = make_node(nodes, NodeType::ENTRY, "entry");
	entry_reg->add_node(entry);

	NodeRef param_a = make_node(nodes, NodeType::PARAM, "a", { entry });
	NodeRef param_b = make_node(nodes, NodeType::PARAM, "b", { entry });
	entry_reg->add_node(param_a);
	entry_reg->add_node(param_b);

	/* lits */
	NodeRef const10 = make_node(nodes, NodeType::LIT, "const10");
	set_const(nodes, const10, 10);
	NodeRef const20 = make_node(nodes, NodeType::LIT, "const20");
	set_const(nodes, const20, 20);
	NodeRef const30 = make_node(nodes, NodeType::LIT, "const30");
	set_const(nodes, const30, 30);
	entry_reg->add_node(const10);
	entry_reg->add_node(const20);
	entry_reg->add_node(const30);

	/* cond: a > b */
	NodeRef condition = make_node(nodes, NodeType::GT, "a_gt_b", { param_a, param_b });
	cond_reg->add_node(condition);

	/* br */
	NodeRef control = make_node(nodes, NodeType::CONTROL, "br_ctrl", { condition });
	cond_reg->add_node(control);

	/* live: then */
	NodeRef then_add = make_node(nodes, NodeType::ADD, "a_plus_10", { param_a, const10 });
	then_reg->add_node(then_add);

	/* dead: then */
	const NodeRef then_dead = make_node(nodes, NodeType::MUL, "dead_then_mul", { param_b, const20 });
	then_reg->add_node(then_dead);

	/* alive: else */
	NodeRef else_mul = make_node(nodes, NodeType::MUL, "b_times_20", { param_b, const20 });
	else_reg->add_node(else_mul);

	/* dead: else */
	const NodeRef else_dead = make_node(nodes, NodeType::ADD, "dead_else_add", { param_a, const30 });
	else_reg->add_node(else_dead);

	/* phi merge */
	NodeRef phi = make_node(nodes, NodeType::PHI, "result_phi", { then_add, else_mul, control });
	exit_reg->add_node(phi);

	/* phi */
	NodeRef ret = make_node(nodes, NodeType::RET, "return", { phi });
	exit_reg->add_node(ret);

	const NodeRef exit = make_node(nodes, NodeType::EXIT, "exit", { ret });
	exit_reg->add_node(exit);
}

int main()
{
	std::vector<std::unique_ptr<SproutNode<> > > nodes1;
	const auto root1 = std::make_shared<SproutRegion>("root");
	root1->set_type(RegionType::ROOT);

	create_test_ir(nodes1, root1);

	std::cout << "before DCE optimization 1: \n";
	dump_ir(root1, nodes1);

	DCEPass dce1;
	dce1.run(root1, nodes1);

	std::cout << "after DCE optimization 1: \n";
	DCEPass::dump_results(dce1.get_dead_nodes(), nodes1, root1);

	/********/

	std::vector<std::unique_ptr<SproutNode<> > > nodes2;
	const auto root2 = std::make_shared<SproutRegion>("root");
	root2->set_type(RegionType::ROOT);

	create_br_ir(nodes2, root2);

	std::cout << "before DCE optimization 1: \n";
	dump_ir(root2, nodes2);

	DCEPass dce2;
	dce2.run(root2, nodes2);

	std::cout << "after DCE optimization 1: \n";
	DCEPass::dump_results(dce2.get_dead_nodes(), nodes2, root2);

	return 0;
}
