#pragma once

#include <cstdint>

namespace sprk::riscv
{
    /* Shift Left Logical */
    uint32_t encode_sll(int rd, int rs1, int rs2);

    /* Shift Right Logical */
    uint32_t encode_srl(int rd, int rs1, int rs2);

    /* Shift Right Arithmetic */
    uint32_t encode_sra(int rd, int rs1, int rs2);

    /* Shift Left Logical Immediate */
    uint32_t encode_slli(int rd, int rs1, int shamt);

    /* Shift Right Logical Immediate */
    uint32_t encode_srli(int rd, int rs1, int shamt);

    /* Shift Right Arithmetic Immediate */
    uint32_t encode_srai(int rd, int rs1, int shamt);

    /* Shift Left Logical Word [RV64I only] */
    uint32_t encode_sllw(int rd, int rs1, int rs2, bool is64bit);

    /* Shift Right Logical Word [RV64I only] */
    uint32_t encode_srlw(int rd, int rs1, int rs2, bool is64bit);

    /* Shift Right Arithmetic Word [RV64I only] */
    uint32_t encode_sraw(int rd, int rs1, int rs2, bool is64bit);
}
