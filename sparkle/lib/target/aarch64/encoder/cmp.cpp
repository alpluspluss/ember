#include <sparkle/target/aarch64/encoding/cmp.hpp>

namespace sprk::aarch64
{
    /* compare instructions are aliases of SUBS with destination register XZR/WZR */
    uint32_t encode_cmp_imm(const int rn, const int imm12, const bool is64bit)
    {
        /* CMP is an alias of SUBS with Rd=XZR/WZR */
        const uint32_t sf = is64bit ? 1 : 0; /* 64-bit (1) or 32-bit (0) */
        constexpr uint32_t op = 1;           /* SUB operation (1) */
        constexpr uint32_t S = 1;            /* set flags (always for CMP) */
        constexpr uint32_t rd = 31;          /* XZR/WZR (register 31) */

        return (sf << 31) |              /* size flag */
               (op << 30) |              /* operation: 1 for SUB */
               (S << 29) |               /* set flags */
               (0b10001 << 24) |         /* opcode fixed pattern */
               (0 << 22) |               /* shift amount */
               ((imm12 & 0xFFF) << 10) | /* 12-bit immediate */
               (rn << 5) |               /* register to compare */
               rd;                       /* XZR/WZR as destination */
    }

    uint32_t encode_cmp_reg(const int rn, const int rm, const uint32_t shift_type, const uint32_t shift_amount,
                            const bool is64bit)
    {
        /* CMP is an alias of SUBS with Rd=XZR/WZR */
        const uint32_t sf = is64bit ? 1 : 0; /* 64-bit (1) or 32-bit (0) */
        constexpr uint32_t op = 1;           /* SUB operation (1) */
        constexpr uint32_t S = 1;            /* set flags (always for CMP) */
        constexpr uint32_t rd = 31;          /* XZR/WZR (register 31) */

        /* shift type: 00=LSL, 01=LSR, 10=ASR, 11=Reserved */
        /* check for valid shift */
        if (is64bit == false && shift_amount >= 32)
            /* for 32-bit registers, shift amount must be < 32 */
            return 0; /* error condition */

        return (sf << 31) |                    /* size flag */
               (op << 30) |                    /* operation: 1 for SUB */
               (S << 29) |                     /* set flags */
               (0b01011 << 24) |               /* opcode fixed pattern */
               (shift_type << 22) |            /* shift type */
               (0 << 21) |                     /* reserved bit */
               (rm << 16) |                    /* second register */
               ((shift_amount & 0x3F) << 10) | /* shift amount */
               (rn << 5) |                     /* first register to compare */
               rd;                             /* XZR/WZR as destination */
    }

    uint32_t encode_tst_reg(const int rn, const int rm, const uint32_t shift_type, const uint32_t shift_amount,
                            const bool is64bit)
    {
        /* TST is an alias of ANDS with Rd=XZR/WZR */
        const uint32_t sf = is64bit ? 1 : 0; /* 64-bit (1) or 32-bit (0) */
        constexpr uint32_t opc = 3;          /* ANDS=3 */
        constexpr uint32_t N = 0;            /* don't invert rm */
        constexpr uint32_t rd = 31;          /* XZR/WZR (register 31) */

        /* shift type: 00=LSL, 01=LSR, 10=ASR, 11=ROR */
        /* check for valid shift */
        if (is64bit == false && shift_amount >= 32)
            /* for 32-bit registers, shift amount must be < 32 */
            return 0; /* error condition */

        return (sf << 31) |                    /* size flag */
               (opc << 29) |                   /* operation: 3 for ANDS */
               (0b01010 << 24) |               /* opcode fixed pattern */
               (shift_type << 22) |            /* shift type */
               (N << 21) |                     /* N bit (0) */
               (rm << 16) |                    /* second register */
               ((shift_amount & 0x3F) << 10) | /* shift amount */
               (rn << 5) |                     /* first register to test */
               rd;                             /* XZR/WZR as destination */
    }
}
