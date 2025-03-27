#pragma once

#include <cstdint>

namespace sprk::riscv
{
    /* Bitwise AND */
    uint32_t encode_band(int rd, int rs1, int rs2);

    /* Bitwise OR */
    uint32_t encode_bor(int rd, int rs1, int rs2);

    /* Bitwise XOR */
    uint32_t encode_bxor(int rd, int rs1, int rs2);

    /* Bitwise AND with immediate */
    uint32_t encode_bandi(int rd, int rs1, int imm);

    /* Bitwise OR with immediate */
    uint32_t encode_bori(int rd, int rs1, int imm);

    /* Bitwise XOR with immediate */
    uint32_t encode_bxori(int rd, int rs1, int imm);
}
