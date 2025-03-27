#include <sparkle/target/risc-v/encoding/atomic.hpp>

namespace sprk::riscv
{
    uint32_t encode_lr_w(int rd, int rs1, bool aq, bool rl)
    {
        return ((aq ? 1 : 0) << 26) |   /* acquire */
               ((rl ? 1 : 0) << 25) |   /* release */
               (0b00010 << 20) |        /* rs2 = 0 for LR */
               (rs1 << 15) |            /* base address register */
               (0b010 << 12) |          /* funct3 = word */
               (rd << 7) |              /* destination register */
               0b0101111;               /* opcode (AMO) */
    }

    uint32_t encode_sc_w(int rd, int rs1, int rs2, bool aq, bool rl)
    {
        return ((aq ? 1 : 0) << 26) |   /* acquire */
               ((rl ? 1 : 0) << 25) |   /* release */
               (rs2 << 20) |            /* source register */
               (rs1 << 15) |            /* base address register */
               (0b010 << 12) |          /* funct3 = word */
               (rd << 7) |              /* destination register */
               0b0101111;               /* opcode (AMO) */
    }

    uint32_t encode_amoswap_w(int rd, int rs1, int rs2, bool aq, bool rl)
    {
        return ((aq ? 1 : 0) << 26) |   /* acquire */
               ((rl ? 1 : 0) << 25) |   /* release */
               (rs2 << 20) |            /* source register */
               (rs1 << 15) |            /* base address register */
               (0b010 << 12) |          /* funct3 = word */
               (rd << 7) |              /* destination register */
               (0b00001 << 27) |        /* funct5 = SWAP */
               0b0101111;               /* opcode (AMO) */
    }

    uint32_t encode_amoadd_w(int rd, int rs1, int rs2, bool aq, bool rl)
    {
        return ((aq ? 1 : 0) << 26) |   /* acquire */
               ((rl ? 1 : 0) << 25) |   /* release */
               (rs2 << 20) |            /* source register */
               (rs1 << 15) |            /* base address register */
               (0b010 << 12) |          /* funct3 = word */
               (rd << 7) |              /* destination register */
               (0b00000 << 27) |        /* funct5 = ADD */
               0b0101111;               /* opcode (AMO) */
    }

    uint32_t encode_amoxor_w(int rd, int rs1, int rs2, bool aq, bool rl)
    {
        return ((aq ? 1 : 0) << 26) |   /* acquire */
               ((rl ? 1 : 0) << 25) |   /* release */
               (rs2 << 20) |            /* source register */
               (rs1 << 15) |            /* base address register */
               (0b010 << 12) |          /* funct3 = word */
               (rd << 7) |              /* destination register */
               (0b00100 << 27) |        /* funct5 = XOR */
               0b0101111;               /* opcode (AMO) */
    }

    uint32_t encode_amoand_w(int rd, int rs1, int rs2, bool aq, bool rl)
    {
        return ((aq ? 1 : 0) << 26) |   /* acquire */
               ((rl ? 1 : 0) << 25) |   /* release */
               (rs2 << 20) |            /* source register */
               (rs1 << 15) |            /* base address register */
               (0b010 << 12) |          /* funct3 = word */
               (rd << 7) |              /* destination register */
               (0b01100 << 27) |        /* funct5 = AND */
               0b0101111;               /* opcode (AMO) */
    }

    uint32_t encode_amoor_w(int rd, int rs1, int rs2, bool aq, bool rl)
    {
        return ((aq ? 1 : 0) << 26) |   /* acquire */
               ((rl ? 1 : 0) << 25) |   /* release */
               (rs2 << 20) |            /* source register */
               (rs1 << 15) |            /* base address register */
               (0b010 << 12) |          /* funct3 = word */
               (rd << 7) |              /* destination register */
               (0b01000 << 27) |        /* funct5 = OR */
               0b0101111;               /* opcode (AMO) */
    }

    uint32_t encode_amomin_w(int rd, int rs1, int rs2, bool aq, bool rl)
    {
        return ((aq ? 1 : 0) << 26) |   /* acquire */
               ((rl ? 1 : 0) << 25) |   /* release */
               (rs2 << 20) |            /* source register */
               (rs1 << 15) |            /* base address register */
               (0b010 << 12) |          /* funct3 = word */
               (rd << 7) |              /* destination register */
               (0b10000 << 27) |        /* funct5 = MIN */
               0b0101111;               /* opcode (AMO) */
    }

    uint32_t encode_amomax_w(int rd, int rs1, int rs2, bool aq, bool rl)
    {
        return ((aq ? 1 : 0) << 26) |   /* acquire */
               ((rl ? 1 : 0) << 25) |   /* release */
               (rs2 << 20) |            /* source register */
               (rs1 << 15) |            /* base address register */
               (0b010 << 12) |          /* funct3 = word */
               (rd << 7) |              /* destination register */
               (0b10100 << 27) |        /* funct5 = MAX */
               0b0101111;               /* opcode (AMO) */
    }
}