#include <iostream>
#include <sparkle/sprout/passes/pre.hpp>
#include <sparkle/sprout/passes/dce.hpp>
#include <sparkle/sprout/utils/dump.hpp>

using namespace sprk;

NodeRef make_node(std::vector<std::unique_ptr<SproutNode<>>> &nodes,
                  const NodeType type,
                  const std::string &name = "",
                  const std::vector<NodeRef> &inputs = {})
{
    const auto id = static_cast<NodeRef>(nodes.size());
    auto node = std::make_unique<SproutNode<>>();

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

void set_const(const std::vector<std::unique_ptr<SproutNode<>>> &nodes,
               const NodeRef node_ref,
               int64_t value)
{
    if (node_ref < nodes.size() && nodes[node_ref])
        nodes[node_ref]->value = value;
}

void set_fn_ref(const std::vector<std::unique_ptr<SproutNode<>>> &nodes,
                const NodeRef node_ref,
                const NodeRef fn_ref)
{
    if (node_ref < nodes.size() && nodes[node_ref])
        nodes[node_ref]->fn_ref = fn_ref;
}

void pre_test_ir(std::vector<std::unique_ptr<SproutNode<>>> &nodes,
                 const std::shared_ptr<SproutRegion> &root_region)
{
    auto function_region = std::make_shared<SproutRegion>("test_function");
    function_region->set_type(RegionType::FUNCTION);
    root_region->add_child(function_region);

    auto entry_region = std::make_shared<SproutRegion>("entry");
    entry_region->set_type(RegionType::BASIC_BLOCK);
    function_region->add_child(entry_region);

    auto if_condition_region = std::make_shared<SproutRegion>("if_condition");
    if_condition_region->set_type(RegionType::BASIC_BLOCK);
    function_region->add_child(if_condition_region);

    auto then_region = std::make_shared<SproutRegion>("then_branch");
    then_region->set_type(RegionType::BRANCH_THEN);
    function_region->add_child(then_region);

    auto else_region = std::make_shared<SproutRegion>("else_branch");
    else_region->set_type(RegionType::BRANCH_ELSE);
    function_region->add_child(else_region);

    auto exit_region = std::make_shared<SproutRegion>("exit");
    exit_region->set_type(RegionType::BASIC_BLOCK);
    function_region->add_child(exit_region);

    entry_region->set_imm_dominator(function_region);
    if_condition_region->set_imm_dominator(entry_region);
    then_region->set_imm_dominator(if_condition_region);
    else_region->set_imm_dominator(if_condition_region);
    exit_region->set_imm_dominator(if_condition_region);

    NodeRef function_node = make_node(nodes, NodeType::FUNCTION, "test_function");
    function_region->add_node(function_node);

    NodeRef entry_node = make_node(nodes, NodeType::ENTRY, "entry");
    entry_region->add_node(entry_node);
    set_fn_ref(nodes, entry_node, function_node);

    NodeRef param_a = make_node(nodes, NodeType::PARAM, "a", {entry_node});
    NodeRef param_b = make_node(nodes, NodeType::PARAM, "b", {entry_node});
    entry_region->add_node(param_a);
    entry_region->add_node(param_b);
    set_fn_ref(nodes, param_a, function_node);
    set_fn_ref(nodes, param_b, function_node);

    NodeRef const_10 = make_node(nodes, NodeType::CONST, "const10");
    set_const(nodes, const_10, 10);
    entry_region->add_node(const_10);

    NodeRef condition = make_node(nodes, NodeType::CMP, "a < b", {param_a, param_b});
    if_condition_region->add_node(condition);
    set_fn_ref(nodes, condition, function_node);

    const NodeRef control = make_node(nodes, NodeType::CONTROL, "if_control", {condition});
    if_condition_region->add_node(control);
    set_fn_ref(nodes, control, function_node);
    then_region->set_ctrl_deps(control);
    else_region->set_ctrl_deps(control);

    NodeRef mul_then = make_node(nodes, NodeType::MUL, "a * 10 (then)", {param_a, const_10});
    then_region->add_node(mul_then);
    set_fn_ref(nodes, mul_then, function_node);

    NodeRef add_then = make_node(nodes, NodeType::ADD, "a*10 + b (then)", {mul_then, param_b});
    then_region->add_node(add_then);
    set_fn_ref(nodes, add_then, function_node);

    // Create same redundant computation in else branch: a * 10
    NodeRef mul_else = make_node(nodes, NodeType::MUL, "a * 10 (else)", {param_a, const_10});
    else_region->add_node(mul_else);
    set_fn_ref(nodes, mul_else, function_node);

    NodeRef sub_else = make_node(nodes, NodeType::SUB, "a*10 - b (else)", {mul_else, param_b});
    else_region->add_node(sub_else);
    set_fn_ref(nodes, sub_else, function_node);

    NodeRef phi_result = make_node(nodes, NodeType::PHI, "result_phi", {add_then, sub_else});
    exit_region->add_node(phi_result);
    set_fn_ref(nodes, phi_result, function_node);

    NodeRef mul_exit = make_node(nodes, NodeType::MUL, "a * 10 (exit)", {param_a, const_10});
    exit_region->add_node(mul_exit);
    set_fn_ref(nodes, mul_exit, function_node);

    NodeRef add_exit = make_node(nodes, NodeType::ADD, "phi + a*10", {phi_result, mul_exit});
    exit_region->add_node(add_exit);
    set_fn_ref(nodes, add_exit, function_node);

    NodeRef ret_node = make_node(nodes, NodeType::RET, "return", {add_exit});
    exit_region->add_node(ret_node);
    set_fn_ref(nodes, ret_node, function_node);
}

int main()
{
    std::vector<std::unique_ptr<SproutNode<>>> nodes;
    const auto root = std::make_shared<SproutRegion>("root");
    root->set_type(RegionType::ROOT);

    pre_test_ir(nodes, root);

    std::cout << "original:\n";
    dump_ir(root, nodes);
    std::cout << "\n";

    PREPass pre;
    pre.run(root, nodes);

    std::cout << "\nbefore PRE:\n";
    pre.dump_results(true);
    std::cout << "\n";

    std::cout << "\nafter PRE\n";
    dump_ir(root, nodes);
    std::cout << "\n";

    DCEPass dce;
    dce.run(root, nodes);

    std::cout << "\nfinal result:\n";
    dce.dump_results(dce.get_dead_nodes(), nodes, root, true);
    std::cout << "\n";

    return 0;
}
