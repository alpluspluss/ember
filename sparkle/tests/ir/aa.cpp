#include <iostream>
#include <sparkle/ir/passes/aa.hpp>
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
               const NodeRef node_ref,
               int64_t value)
{
	if (node_ref < nodes.size() && nodes[node_ref])
		nodes[node_ref]->value = value;
}

void set_mem_obj(const std::vector<std::unique_ptr<SproutNode<> > > &nodes,
                 const NodeRef node_ref,
                 const NodeRef mem_obj)
{
	if (node_ref < nodes.size() && nodes[node_ref])
		nodes[node_ref]->mem_obj = mem_obj;
}

void create_memory_test_ir(std::vector<std::unique_ptr<SproutNode<> > > &nodes,
                           const std::shared_ptr<SproutRegion> &root_region)
{
	const auto fn_reg = std::make_shared<SproutRegion>("memtest");
	fn_reg->set_type(RegionType::FUNCTION);

	const auto entry_reg = std::make_shared<SproutRegion>("entry");
	entry_reg->set_type(RegionType::BASIC_BLOCK);

	const auto body_reg = std::make_shared<SproutRegion>("body");
	body_reg->set_type(RegionType::BASIC_BLOCK);

	const auto exit_reg = std::make_shared<SproutRegion>("exit");
	exit_reg->set_type(RegionType::BASIC_BLOCK);

	root_region->add_child(fn_reg);
	fn_reg->add_child(entry_reg);
	fn_reg->add_child(body_reg);
	fn_reg->add_child(exit_reg);

	NodeRef entry = make_node(nodes, NodeType::ENTRY, "entry");
	entry_reg->add_node(entry);

	NodeRef local_var = make_node(nodes, NodeType::PARAM, "local_var");
	entry_reg->add_node(local_var);

	/* take address of local variable (creates a stack pointer) */
	NodeRef addr_of_local = make_node(nodes, NodeType::ADDR_OF, "ptr_to_local", { local_var });
	entry_reg->add_node(addr_of_local);

	/* issue 1: uninit read */
	const NodeRef uninit_read = make_node(nodes, NodeType::LOAD, "uninitialized_read", { addr_of_local });
	set_mem_obj(nodes, uninit_read, addr_of_local);
	body_reg->add_node(uninit_read);

	NodeRef const42 = make_node(nodes, NodeType::LIT, "const42");
	set_const(nodes, const42, 42);
	entry_reg->add_node(const42);

	const NodeRef store_local = make_node(nodes, NodeType::STORE, "initialize_local", { addr_of_local, const42 });
	set_mem_obj(nodes, store_local, addr_of_local);
	body_reg->add_node(store_local);

	NodeRef heap_alloc = make_node(nodes, NodeType::MALLOC, "heap_ptr");
	body_reg->add_node(heap_alloc);

	const NodeRef store_heap = make_node(nodes, NodeType::STORE, "store_to_heap", { heap_alloc, const42 });
	set_mem_obj(nodes, store_heap, heap_alloc);
	body_reg->add_node(store_heap);

	/* issue 2: double free */
	const NodeRef free1 = make_node(nodes, NodeType::FREE, "free_heap_1", { heap_alloc });
	set_mem_obj(nodes, free1, heap_alloc);
	body_reg->add_node(free1);

	const NodeRef free2 = make_node(nodes, NodeType::FREE, "free_heap_2", { heap_alloc });
	set_mem_obj(nodes, free2, heap_alloc);
	body_reg->add_node(free2);

	NodeRef heap_alloc2 = make_node(nodes, NodeType::MALLOC, "heap_ptr2");
	body_reg->add_node(heap_alloc2);

	/* store to second heap alloc */
	const NodeRef store_heap2 = make_node(nodes, NodeType::STORE, "store_to_heap2", { heap_alloc2, const42 });
	set_mem_obj(nodes, store_heap2, heap_alloc2);
	body_reg->add_node(store_heap2);

	/* free second */
	const NodeRef free_heap2 = make_node(nodes, NodeType::FREE, "free_heap2", { heap_alloc2 });
	set_mem_obj(nodes, free_heap2, heap_alloc2);
	body_reg->add_node(free_heap2);

	/* issue 3: use after free */
	NodeRef use_after_free = make_node(nodes, NodeType::LOAD, "use_after_free", { heap_alloc2 });
	set_mem_obj(nodes, use_after_free, heap_alloc2);
	body_reg->add_node(use_after_free);

	NodeRef ret = make_node(nodes, NodeType::RET, "return", { use_after_free });
	exit_reg->add_node(ret);

	const NodeRef exit = make_node(nodes, NodeType::EXIT, "exit", { ret });
	exit_reg->add_node(exit);
}

int main()
{
	std::vector<std::unique_ptr<SproutNode<> > > nodes1;
	const auto root1 = std::make_shared<SproutRegion>("root");
	root1->set_type(RegionType::ROOT);

	create_memory_test_ir(nodes1, root1);

	std::cout << "before analysis result: \n";
	dump_ir(root1, nodes1);
	AliasAnalysisPass aa;
	aa.run(root1, nodes1);

	std::cout << "alias analysis result: \n";
	aa.dump_results(nodes1, root1.get());

	return 0;
}