#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <string>
#include <variant>

namespace sprk
{
    using NodeRef = uint32_t;
    inline constexpr NodeRef NULL_REF = UINT32_MAX; /* sentinel value; if this node is null */

    enum class NodeProps : uint8_t
    {
        NONE = 0,
        STATIC = 1 << 0, /* placed in the static section */
        VOLATILE = 1 << 1, /* observable */
        CONSTEXPR = 1 << 2, /* compile-time evaluation */
        EXTERN = 1 << 3, /* C ABI */
        /* TBA... */
    };

    inline NodeProps operator|(NodeProps lhs, NodeProps rhs)
    {
        return static_cast<NodeProps>(
            static_cast<std::underlying_type_t<NodeProps>>(lhs) |
            static_cast<std::underlying_type_t<NodeProps>>(rhs));
    }

    inline NodeProps& operator|=(NodeProps& lhs, NodeProps rhs)
    {
        lhs = lhs | rhs;
        return lhs;
    }

    inline NodeProps operator&(NodeProps lhs, NodeProps rhs)
    {
        return static_cast<NodeProps>(
            static_cast<std::underlying_type_t<NodeProps>>(lhs) &
            static_cast<std::underlying_type_t<NodeProps>>(rhs));
    }

    inline NodeProps& operator&=(NodeProps& lhs, NodeProps rhs)
    {
        lhs = lhs & rhs;
        return lhs;
    }

    inline NodeProps operator^(NodeProps lhs, NodeProps rhs)
    {
        return static_cast<NodeProps>(
            static_cast<std::underlying_type_t<NodeProps>>(lhs) ^
            static_cast<std::underlying_type_t<NodeProps>>(rhs));
    }

    inline NodeProps& operator^=(NodeProps& lhs, NodeProps rhs)
    {
        lhs = lhs ^ rhs;
        return lhs;
    }

    inline NodeProps operator~(NodeProps prop)
    {
        return static_cast<NodeProps>(
            ~static_cast<std::underlying_type_t<NodeProps>>(prop));
    }

    enum class NodeType : uint16_t
    {
        /* primitives & boundaries */
        ENTRY,
        EXIT,
        PARAM,
        LIT, /* representation of values, regardless 
                * if it's a `const` or not as that should be resolved at AST */
        
        /* operations */
        ADD,
        SUB,
        MUL,
        DIV,
        GT,
        GTE,
        LT,
        LTE,
        EQ,
        NEQ,
        BAND,
        BOR,
        BXOR,
        BNOT,
        BSHL,
        BSHR,
        PHI,
        CONTROL,
        RET,

        /* functions */
        FUNCTION,
        CALL,
        CALL_PARAM,
        CALL_RESULT,

        /* memory operations */
        MALLOC,
        FREE,
        LOAD,
        STORE,

        /* pointer operations */
        ADDR_OF, /* &var */
        PTR_LOAD, /* *ptr; load via ptr */
        PTR_STORE, /* *ptr = val; store via ptr */
        PTR_ADD, /* ptr + offset; ptr arithmetic */

        /* casting */
        REINTERPRET_CAST,
    };

    template<size_t MAX_INPUTS = 4, size_t MAX_USERS = 4>
    struct SproutNode
    {
        using NodeValue = std::variant<std::monostate, int64_t, double, void*, NodeRef, std::string>;

        size_t expr_hash = 0;
        uint64_t str_id = {};
        NodeValue value = {};

        NodeRef inputs[MAX_INPUTS] = {};
        NodeRef users[MAX_USERS] = {};
        NodeRef id = {};
        NodeRef fn_ref = {};
        NodeRef mem_obj = {};
        uint32_t flags = {};
        NodeType type = {};
        NodeProps props = {};
        uint8_t input_count = {};
        uint8_t user_count = {};

        SproutNode() = default;
        SproutNode(const SproutNode&) = delete;
        SproutNode& operator=(const SproutNode&) = delete;

        SproutNode(SproutNode&&) = default;
        SproutNode& operator=(SproutNode&&) = default;
    };

    static_assert(std::is_standard_layout_v<SproutNode<>>, "SproutNode must have standard layout");
    static_assert(std::is_move_constructible_v<SproutNode<>>, "SproutNode must be move constructible");

    /* aliases */
    constexpr inline NodeProps NONE = NodeProps::NONE;
    constexpr inline NodeProps STATIC = NodeProps::STATIC;
    constexpr inline NodeProps VOLATILE = NodeProps::VOLATILE;
    constexpr inline NodeProps CONSTEXPR = NodeProps::CONSTEXPR;
    constexpr inline NodeProps EXTERN = NodeProps::EXTERN;

    constexpr inline NodeType IR_ENTRY = NodeType::ENTRY;
    constexpr inline NodeType IR_EXIT = NodeType::EXIT;
    constexpr inline NodeType IR_PARAM = NodeType::PARAM;
    constexpr inline NodeType IR_LIT = NodeType::LIT;
    constexpr inline NodeType IR_ADD = NodeType::ADD;
    constexpr inline NodeType IR_SUB = NodeType::SUB;
    constexpr inline NodeType IR_MUL = NodeType::MUL;
    constexpr inline NodeType IR_DIV = NodeType::DIV;
    constexpr inline NodeType IR_GT = NodeType::GT;
    constexpr inline NodeType IR_GTE = NodeType::GTE;
    constexpr inline NodeType IR_LT = NodeType::LT;
    constexpr inline NodeType IR_LTE = NodeType::LTE;
    constexpr inline NodeType IR_EQ = NodeType::EQ;
    constexpr inline NodeType IR_NEQ = NodeType::NEQ;
    constexpr inline NodeType IR_BAND = NodeType::BAND;
    constexpr inline NodeType IR_BOR = NodeType::BOR;
    constexpr inline NodeType IR_BXOR = NodeType::BXOR;
    constexpr inline NodeType IR_BNOT = NodeType::BNOT;
    constexpr inline NodeType IR_BSHL = NodeType::BSHL;
    constexpr inline NodeType IR_BSHR = NodeType::BSHR;
    constexpr inline NodeType IR_PHI = NodeType::PHI;
    constexpr inline NodeType IR_CONTROL = NodeType::CONTROL;
    constexpr inline NodeType IR_RET = NodeType::RET;
    constexpr inline NodeType IR_FUNCTION = NodeType::FUNCTION;
    constexpr inline NodeType IR_CALL = NodeType::CALL;
    constexpr inline NodeType IR_CALL_PARAM = NodeType::CALL_PARAM;
    constexpr inline NodeType IR_CALL_RESULT = NodeType::CALL_RESULT;
    constexpr inline NodeType IR_MALLOC = NodeType::MALLOC;
    constexpr inline NodeType IR_FREE = NodeType::FREE;
    constexpr inline NodeType IR_LOAD = NodeType::LOAD;
    constexpr inline NodeType IR_STORE = NodeType::STORE;
    constexpr inline NodeType IR_ADDR_OF = NodeType::ADDR_OF;
    constexpr inline NodeType IR_PTR_LOAD = NodeType::PTR_LOAD;
    constexpr inline NodeType IR_PTR_STORE = NodeType::PTR_STORE;
    constexpr inline NodeType IR_PTR_ADD = NodeType::PTR_ADD;
    constexpr inline NodeType IR_REINTERPRET_CAST = NodeType::REINTERPRET_CAST;
}
