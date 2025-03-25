#pragma once

#include <cstdint>

namespace sprk::riscv
{
    /* Multiply */
    uint32_t encode_mul(int rd, int rs1, int rs2);

    /* Multiply High Signed x Signed */
    uint32_t encode_mulh(int rd, int rs1, int rs2);

    /* Multiply High Signed x Unsigned */
    uint32_t encode_mulhsu(int rd, int rs1, int rs2);

    /* Multiply High Unsigned x Unsigned */
    uint32_t encode_mulhu(int rd, int rs1, int rs2);

    /* Divide Signed */
    uint32_t encode_div(int rd, int rs1, int rs2);

    /* Divide Unsigned */
    uint32_t encode_divu(int rd, int rs1, int rs2);

    /* Remainder Signed */
    uint32_t encode_rem(int rd, int rs1, int rs2);

    /* Remainder Unsigned */
    uint32_t encode_remu(int rd, int rs1, int rs2);

    /* Multiply Word [RV64 only] */
    uint32_t encode_mulw(int rd, int rs1, int rs2);
}
