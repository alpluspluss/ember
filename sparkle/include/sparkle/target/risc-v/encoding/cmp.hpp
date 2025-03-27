#pragma once

#include <cstdint>

namespace sprk::riscv
{
    /* Set if Less Than */
    uint32_t encode_slt(int rd, int rs1, int rs2);

    /* Set if Less Than Unsigned */
    uint32_t encode_sltu(int rd, int rs1, int rs2);

    /* Set if Less Than Immediate */
    uint32_t encode_slti(int rd, int rs1, int imm);

    /* Set if Less Than Immediate Unsigned */
    uint32_t encode_sltiu(int rd, int rs1, int imm);
}
