#pragma once

#include <cstdint>

namespace sprk::riscv
{
    /* Count Leading Zero */
    uint32_t encode_clz(int rd, int rs1);

    /* Count Trailing Zero */
    uint32_t encode_ctz(int rd, int rs1);

    /* Count Population */
    uint32_t encode_cpop(int rd, int rs1);

    /* AND with inverted operand */
    uint32_t encode_andn(int rd, int rs1, int rs2);

    /* OR with inverted operand */
    uint32_t encode_orn(int rd, int rs1, int rs2);

    /* XOR with inverted operand */
    uint32_t encode_xnor(int rd, int rs1, int rs2);

    /* Bit Extract */
    uint32_t encode_bext(int rd, int rs1, int rs2);

    /* Bit Clear */
    uint32_t encode_bclr(int rd, int rs1, int rs2);

    /* Bit Set */
    uint32_t encode_bset(int rd, int rs1, int rs2);

    /* Bit Invert */
    uint32_t encode_binv(int rd, int rs1, int rs2);
}
