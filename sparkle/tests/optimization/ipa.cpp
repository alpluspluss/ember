#include <iostream>
#include <sparkle/sprout/passes/ipa.hpp>
#include <sparkle/sprout/utils/dump.hpp>

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
        node->string_id = reinterpret_cast<uint64_t>(strdup(name.c_str()));

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

void set_fn_ref(const std::vector<std::unique_ptr<SproutNode<> > > &nodes,
                const NodeRef node_ref,
                const NodeRef fn_ref)
{
    if (node_ref < nodes.size() && nodes[node_ref])
        nodes[node_ref]->fn_ref = fn_ref;
}

void interproc_test_ir(std::vector<std::unique_ptr<SproutNode<> > > &nodes,
                                    const std::shared_ptr<SproutRegion> &root_region)
{
    auto main_fn_reg = std::make_shared<SproutRegion>("main_function");
    main_fn_reg->set_type(RegionType::FUNCTION);

    auto square_fn_reg = std::make_shared<SproutRegion>("square_function");
    square_fn_reg->set_type(RegionType::FUNCTION);

    auto cube_fn_reg = std::make_shared<SproutRegion>("cube_function");
    cube_fn_reg->set_type(RegionType::FUNCTION);

    root_region->add_child(main_fn_reg);
    root_region->add_child(square_fn_reg);
    root_region->add_child(cube_fn_reg);

    NodeRef square_fn = make_node(nodes, NodeType::FUNCTION, "square");
    square_fn_reg->add_node(square_fn);

    NodeRef square_entry = make_node(nodes, NodeType::ENTRY, "square_entry");
    square_fn_reg->add_node(square_entry);
    set_fn_ref(nodes, square_entry, square_fn);

    NodeRef square_param = make_node(nodes, NodeType::PARAM, "x", { square_entry });
    square_fn_reg->add_node(square_param);
    set_fn_ref(nodes, square_param, square_fn);

    NodeRef square_mul = make_node(nodes, NodeType::MUL, "x * x", { square_param, square_param });
    square_fn_reg->add_node(square_mul);
    set_fn_ref(nodes, square_mul, square_fn);

    NodeRef square_ret = make_node(nodes, NodeType::RET, "square_return", { square_mul });
    square_fn_reg->add_node(square_ret);
    set_fn_ref(nodes, square_ret, square_fn);

    NodeRef cube_fn = make_node(nodes, NodeType::FUNCTION, "cube");
    cube_fn_reg->add_node(cube_fn);

    NodeRef cube_entry = make_node(nodes, NodeType::ENTRY, "cube_entry");
    cube_fn_reg->add_node(cube_entry);
    set_fn_ref(nodes, cube_entry, cube_fn);

    NodeRef cube_param = make_node(nodes, NodeType::PARAM, "y", { cube_entry });
    cube_fn_reg->add_node(cube_param);
    set_fn_ref(nodes, cube_param, cube_fn);

    NodeRef square_call = make_node(nodes, NodeType::CALL, "call_square", { square_fn, cube_param });
    cube_fn_reg->add_node(square_call);
    set_fn_ref(nodes, square_call, cube_fn);

    NodeRef cube_mul = make_node(nodes, NodeType::MUL, "square(y) * y", { square_call, cube_param });
    cube_fn_reg->add_node(cube_mul);
    set_fn_ref(nodes, cube_mul, cube_fn);

    NodeRef cube_ret = make_node(nodes, NodeType::RET, "cube_return", { cube_mul });
    cube_fn_reg->add_node(cube_ret);
    set_fn_ref(nodes, cube_ret, cube_fn);

    NodeRef main_fn = make_node(nodes, NodeType::FUNCTION, "main");
    main_fn_reg->add_node(main_fn);

    NodeRef main_entry = make_node(nodes, NodeType::ENTRY, "main_entry");
    main_fn_reg->add_node(main_entry);
    set_fn_ref(nodes, main_entry, main_fn);

    NodeRef const5 = make_node(nodes, NodeType::CONST, "const5");
    set_const(nodes, const5, 5);
    main_fn_reg->add_node(const5);

    NodeRef const10 = make_node(nodes, NodeType::CONST, "const10");
    set_const(nodes, const10, 10);
    main_fn_reg->add_node(const10);

    NodeRef square_call_main = make_node(nodes, NodeType::CALL, "call_square_main", { square_fn, const5 });
    main_fn_reg->add_node(square_call_main);
    set_fn_ref(nodes, square_call_main, main_fn);

    NodeRef cube_call_main = make_node(nodes, NodeType::CALL, "call_cube_main", { cube_fn, const10 });
    main_fn_reg->add_node(cube_call_main);
    set_fn_ref(nodes, cube_call_main, main_fn);

    NodeRef add_results = make_node(nodes, NodeType::ADD, "square(5) + cube(10)", { square_call_main, cube_call_main });
    main_fn_reg->add_node(add_results);
    set_fn_ref(nodes, add_results, main_fn);

    NodeRef main_ret = make_node(nodes, NodeType::RET, "main_return", { add_results });
    main_fn_reg->add_node(main_ret);
    set_fn_ref(nodes, main_ret, main_fn);
}

int main()
{
    std::vector<std::unique_ptr<SproutNode<> > > nodes;
    const auto root = std::make_shared<SproutRegion>("root");
    root->set_type(RegionType::ROOT);

    interproc_test_ir(nodes, root);

    std::cout << "before IPA:\n";
    dump_ir(root, nodes);

    IPAPass ipa;
    ipa.run(root, nodes);

    std::cout << "\nIPA results:\n";
    ipa.dump_results();

    return 0;
}
