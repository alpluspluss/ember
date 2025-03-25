#pragma once

#include <cstdint>

namespace sprk::riscv
{
    /* Load Byte */
    uint32_t encode_lb(int rd, int rs1, int offset);

    /* Load Halfword */
    uint32_t encode_lh(int rd, int rs1, int offset);

    /* Load Word */
    uint32_t encode_lw(int rd, int rs1, int offset);

    /* Load Doubleword [RV64 exclusive] */
    uint32_t encode_ld(int rd, int rs1, int offset);

    /* Load Byte Unsigned */
    uint32_t encode_lbu(int rd, int rs1, int offset);

    /* Load Halfword Unsigned */
    uint32_t encode_lhu(int rd, int rs1, int offset);

    /* Load Word Unsigned [RV64 exclusive] */
    uint32_t encode_lwu(int rd, int rs1, int offset);

    /* Store Byte */
    uint32_t encode_sb(int rs2, int rs1, int offset);

    /* Store Halfword */
    uint32_t encode_sh(int rs2, int rs1, int offset);

    /* Store Word */
    uint32_t encode_sw(int rs2, int rs1, int offset);

    /* Store Doubleword [RV64 exclusive] */
    uint32_t encode_sd(int rs2, int rs1, int offset);
}
