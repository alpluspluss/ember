#pragma once

#include <vector>

namespace sprk
{
    /* parallelism etc; this enum is shared with `PassManager` */
    enum class PassFlags
    {
        NONE = 0,
        HORIZONTAL_PARALLELISM = 1 << 0,
        VERTICAL_PARALLELISM = 1 << 1
    };

    inline PassFlags operator|(PassFlags lhs, PassFlags rhs)
    {
        return static_cast<PassFlags>(
            static_cast<std::underlying_type_t<PassFlags>>(lhs) |
            static_cast<std::underlying_type_t<PassFlags>>(rhs));
    }

    inline PassFlags& operator|=(PassFlags& lhs, PassFlags rhs)
    {
        lhs = lhs | rhs;
        return lhs;
    }

    inline PassFlags operator&(PassFlags lhs, PassFlags rhs)
    {
        return static_cast<PassFlags>(
            static_cast<std::underlying_type_t<PassFlags>>(lhs) &
            static_cast<std::underlying_type_t<PassFlags>>(rhs));
    }

    inline PassFlags& operator&=(PassFlags& lhs, PassFlags rhs)
    {
        lhs = lhs & rhs;
        return lhs;
    }

    inline PassFlags operator~(PassFlags flag)
    {
        return static_cast<PassFlags>(
            ~static_cast<std::underlying_type_t<PassFlags>>(flag));
    }

    enum class PassType
    {
        NONE,
        LOCAL,
        INTER,
        WHOLE,
    };

    enum class PassId
    {
        ALIAS_ANALYSIS,
        DEADCODE_ELIMINATION, /* parallelizable in all context */
        INTERPROCEDURAL_ANALYSIS, /* parallelizable */
        INTERPROCEDURAL_OPTIMIZATION, /* dependency: IPA */
        PARTIAL_REDUNDANCY
    };

    struct Pass
    {
        std::vector<PassId> deps; /* dependency */
        PassId id;
        PassFlags flags;
        PassType type;
    };

    class PassManager
    {
    public:
        explicit PassManager(const std::vector<PassId>& passes) : passes(passes) {}

        void run();

        void run_pass(PassId id);

    private:
        std::vector<PassId> passes;
    };
}
