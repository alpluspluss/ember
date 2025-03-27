#pragma once

#include <cstdint>

namespace sprk::riscv
{
    /* Set Vector Length Immediate */
    uint32_t encode_vsetvli(int rd, int rs1, int vtypei)
    {
        return (vtypei << 20) |     /* vector type immediate */
               (rs1 << 15) |        /* rs1 */
               (0b111 << 12) |      /* funct3 */
               (rd << 7) |          /* rd */
               0b1010111;           /* opcode (Vector) */
    }

    /* Vector Load Element, 32-bit */
    uint32_t encode_vle32_v(int vd, int rs1, bool mask_used)
    {
        return (0b000 << 29) |      /* mop = unit-stride */
               (mask_used ? (1 << 25) : 0) | /* mask policy */
               (0b000 << 23) |      /* element width = 32-bit */
               (rs1 << 15) |        /* base address register */
               (0b101 << 12) |      /* funct3 = vector load */
               (vd << 7) |          /* destination vector register */
               0b0000111;           /* opcode (Vector Load) */
    }

    /* Vector Store Element, 32-bit */
    uint32_t encode_vse32_v(int vs3, int rs1, bool mask_used)
    {
        return (0b000 << 29) |      /* mop = unit-stride */
               (mask_used ? (1 << 25) : 0) | /* mask policy */
               (0b000 << 23) |      /* element width = 32-bit */
               (rs1 << 15) |        /* base address register */
               (0b101 << 12) |      /* funct3 = vector store */
               (vs3 << 7) |         /* source vector register */
               0b0100111;           /* opcode (Vector Store) */
    }

    /* Vector Add, Vector-Vector */
    uint32_t encode_vadd_vv(int vd, int vs1, int vs2, bool mask_used)
    {
        return (0b000000 << 26) |   /* funct6 = add */
               (vs2 << 20) |        /* vs2 */
               (vs1 << 15) |        /* vs1 */
               (0b000 << 12) |      /* funct3 = vector-vector */
               (vd << 7) |          /* destination vector register */
               0b1010111;           /* opcode (Vector) */
    }

    /* Vector Multiply, Vector-Vector */
    uint32_t encode_vmul_vv(int vd, int vs1, int vs2, bool mask_used)
    {
        return (0b100101 << 26) |   /* funct6 = multiply */
               (vs2 << 20) |        /* vs2 */
               (vs1 << 15) |        /* vs1 */
               (0b000 << 12) |      /* funct3 = vector-vector */
               (vd << 7) |          /* destination vector register */
               0b1010111;           /* opcode (Vector) */
    }

    /* Vector Floating-point Add, Vector-Vector */
    uint32_t encode_vfadd_vv(int vd, int vs1, int vs2, bool mask_used)
    {
        return (0b000000 << 26) |   /* funct6 = float add */
               (vs2 << 20) |        /* vs2 */
               (vs1 << 15) |        /* vs1 */
               (0b001 << 12) |      /* funct3 = vector-vector floating-point */
               (vd << 7) |          /* destination vector register */
               0b1010111;           /* opcode (Vector) */
    }
}