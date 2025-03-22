#pragma once

#include <cstdint>

namespace sprk::aarch64
{
    uint32_t encode_add_imm(int rd, int rn, int imm12, bool is64bit, bool setflags);
    uint32_t encode_sub_imm(int rd, int rn, int imm12, bool is64bit, bool setflags);
    uint32_t encode_add_reg(int rd, int rn, int rm, uint32_t shift_type, uint32_t shift_amount, bool is64bit, bool setflags);
    uint32_t encode_sub_reg(int rd, int rn, int rm, uint32_t shift_type, uint32_t shift_amount, bool is64bit, bool setflags);
    uint32_t encode_add_ext(int rd, int rn, int rm, uint32_t option, uint32_t imm3, bool is64bit, bool setflags);
    uint32_t encode_sub_ext(int rd, int rn, int rm, uint32_t option, uint32_t imm3, bool is64bit, bool setflags);
}