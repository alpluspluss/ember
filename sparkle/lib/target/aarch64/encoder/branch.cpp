#include <sparkle/target/aarch64/encoding/branch.hpp>

namespace sprk::aarch64
{
    uint32_t encode_b(const int64_t offset)
    {
        /* B has a 26-bit signed immediate offset field, scaled by 4 */
        /* range is ±128MB (±2²⁶ bytes) */
        if (offset < -0x8000000 || offset > 0x7FFFFFF || (offset & 3) != 0)
            return 0; /* error: offset out of range or not aligned to 4 bytes */
        
        uint32_t op = 0;              /* B=0 */
        uint32_t imm26 = (offset >> 2) & 0x3FFFFFF;
        
        return (op << 31) |          /* op bit: 0 for B */
            (0b00101 << 26) |     /* opcode fixed pattern */
            imm26;                /* 26-bit immediate offset divided by 4 */
    }

    uint32_t encode_bl(const int64_t offset)
    {
        /* BL has a 26-bit signed immediate offset field, scaled by 4 */
        /* range is ±128MB (±2²⁶ bytes) */
        if (offset < -0x8000000 || offset > 0x7FFFFFF || (offset & 3) != 0)
            return 0; /* error: offset out of range or not aligned to 4 bytes */

        constexpr uint32_t op = 1;              /* BL=1 */
        const uint32_t imm26 = (offset >> 2) & 0x3FFFFFF;
        
        return (op << 31) |          /* op bit: 1 for BL */
            (0b00101 << 26) |     /* opcode fixed pattern */
            imm26;                /* 26-bit immediate offset divided by 4 */
    }

    uint32_t encode_b_cond(const uint32_t cond, const int64_t offset)
    {
        /* BCOND has a 19-bit signed immediate offset field, scaled by 4 */
        /* range is ±1MB (±2¹⁹ bytes) */
        if (offset < -0x100000 || offset > 0xFFFFF || (offset & 3) != 0)
            return 0; /* error: offset out of range or not aligned to 4 bytes */
        
        /* 'cond' is a 4-bit field encoding the condition */
        /* e.g., 0000=EQ, 0001=NE, 0010=CS/HS, etc. */
        if (cond > 0xF)
            return 0; /* error: invalid condition code */

        const uint32_t imm19 = (offset >> 2) & 0x7FFFF;
        return (0b01010100 << 24) |  /* opcode fixed pattern */
            (imm19 << 5) |        /* 19-bit immediate offset divided by 4 */
            (0 << 4) |            /* 0 for standard B.cond (not BC.cond) */
            cond;                 /* 4-bit condition code */
    }

    uint32_t encode_cbz(const int rd, const int64_t offset, const bool is64bit)
    {
        /* CBZ has a 19-bit signed immediate offset field, scaled by 4 */
        /* range is ±1MB (±2¹⁹ bytes) */
        if (offset < -0x100000 || offset > 0xFFFFF || (offset & 3) != 0)
            return 0; /* error: offset out of range or not aligned to 4 bytes */

        const uint32_t sf = is64bit ? 1 : 0;    /* 64-bit (1) or 32-bit (0) */
        constexpr uint32_t op = 0;                  /* CBZ=0 */
        const uint32_t imm19 = (offset >> 2) & 0x7FFFF;
        
        return (sf << 31) |          /* size flag */
            (0b011010 << 25) |    /* opcode fixed pattern */
            (op << 24) |          /* op bit: 0 for CBZ */
            (imm19 << 5) |        /* 19-bit immediate offset divided by 4 */
            rd;                   /* register to compare */
    }

    uint32_t encode_cbnz(const int rd, const int64_t offset, const bool is64bit)
    {
        /* CBNZ has a 19-bit signed immediate offset field, scaled by 4 */
        /* range is ±1MB (±2¹⁹ bytes) */
        if (offset < -0x100000 || offset > 0xFFFFF || (offset & 3) != 0)
            return 0; /* error: offset out of range or not aligned to 4 bytes */

        const uint32_t sf = is64bit ? 1 : 0;    /* 64-bit (1) or 32-bit (0) */
        constexpr uint32_t op = 1;                  /* CBNZ=1 */
        const uint32_t imm19 = (offset >> 2) & 0x7FFFF;
        
        return (sf << 31) |          /* size flag */
            (0b011010 << 25) |    /* opcode fixed pattern */
            (op << 24) |          /* op bit: 1 for CBNZ */
            (imm19 << 5) |        /* 19-bit immediate offset divided by 4 */
            rd;                   /* register to compare */
    }

    uint32_t encode_tbz(const int rt, const uint32_t bit_pos, const int64_t offset)
    {
        /* TBZ has a 14-bit signed immediate offset field, scaled by 4 */
        /* range is ±32KB (±2¹⁴ bytes) */
        if (offset < -0x8000 || offset > 0x7FFF || (offset & 3) != 0)
            return 0; /* error: offset out of range or not aligned to 4 bytes */
        
        /* bit_pos must be in the range 0-63 */
        if (bit_pos > 63)
            return 0; /* error: bit position out of range */

        constexpr  uint32_t op = 0;                 /* TBZ=0 */
        const uint32_t b5 = (bit_pos >> 5) & 1; /* high bit of the bit number */
        const uint32_t b40 = bit_pos & 0x1F;    /* low 5 bits of the bit number */
        const uint32_t imm14 = (offset >> 2) & 0x3FFF;
        
        return (b5 << 31) |          /* high bit of the bit position */
            (0b011011 << 25) |    /* opcode fixed pattern */
            (op << 24) |          /* op bit: 0 for TBZ */
            (b40 << 19) |         /* low 5 bits of the bit position */
            (imm14 << 5) |        /* 14-bit immediate offset divided by 4 */
            rt;                   /* register to test */
    }

    uint32_t encode_tbnz(const int rt, const uint32_t bit_pos, const int64_t offset)
    {
        /* TBNZ has a 14-bit signed immediate offset field, scaled by 4 */
        /* range is ±32KB (±2¹⁴ bytes) */
        if (offset < -0x8000 || offset > 0x7FFF || (offset & 3) != 0)
            return 0; /* error: offset out of range or not aligned to 4 bytes */
        
        /* bit_pos must be in the range 0-63 */
        if (bit_pos > 63)
            return 0; /* error: bit position out of range */
        
        constexpr uint32_t op = 1;                 /* TBNZ=1 */
        const uint32_t b5 = (bit_pos >> 5) & 1; /* high bit of the bit number */
        const uint32_t b40 = bit_pos & 0x1F;    /* low 5 bits of the bit number */
        const uint32_t imm14 = (offset >> 2) & 0x3FFF;
        
        return (b5 << 31) |          /* high bit of the bit position */
            (0b011011 << 25) |    /* opcode fixed pattern */
            (op << 24) |          /* op bit: 1 for TBNZ */
            (b40 << 19) |         /* low 5 bits of the bit position */
            (imm14 << 5) |        /* 14-bit immediate offset divided by 4 */
            rt;                   /* register to test */
    }
}