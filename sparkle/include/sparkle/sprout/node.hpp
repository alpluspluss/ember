#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <variant>
#include <string>

namespace sprk 
{
    using NodeRef = uint32_t;
    inline constexpr NodeRef NULL_REF = UINT32_MAX;

    enum class NodeType : uint32_t
    {
        /* primitive nodes */
        ENTRY, /* start boundary marker */
        EXIT, /* start boundary marker */
        CONST, /* literal values */
        PARAM, 

        /* operations */
        ADD,
        SUB,
        MUL,
        DIV,
        CMP,
        PHI,
        CONTROL,
        RET,

        /* function-related operations */
        FUNCTION, /* function defintion */
        CALL, /* call to a function */
        CALL_PARAM, /* param for a call */
        CALL_RESULT, /* result of the call */

        /* memory operations */
        MALLOC, /* allocate */
        FREE, /* free */
        LOAD, /* load */
        STORE, /* store */

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

        size_t expr_hash = {};
        uint64_t string_id = {};
        NodeValue value;

        NodeRef inputs[MAX_INPUTS] = {};
        NodeRef users[MAX_USERS] = {};
        NodeRef id = {};
        NodeRef fn_ref = {};
        NodeRef mem_obj = {};
        uint32_t result = {}; /* unique hash for result type */
        uint32_t flags = {};
        NodeType type = {};
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
}

