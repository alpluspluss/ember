#pragma once

#include <cstdint>

namespace sprk::riscv
{
    /* Branch if Equal */
    uint32_t encode_beq(int rs1, int rs2, int offset);

    /* Branch if Not Equal */
    uint32_t encode_bne(int rs1, int rs2, int offset);

    /* Branch if Less Than */
    uint32_t encode_blt(int rs1, int rs2, int offset);

    /* Branch if Greater Than or Equal */
    uint32_t encode_bge(int rs1, int rs2, int offset);

    /* Branch if Less Than Unsigned */
    uint32_t encode_bltu(int rs1, int rs2, int offset);

    /* Branch if Greater Than or Equal Unsigned */
    uint32_t encode_bgeu(int rs1, int rs2, int offset);
}
