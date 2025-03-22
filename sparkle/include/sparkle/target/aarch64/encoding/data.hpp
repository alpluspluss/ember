#pragma once

#include <cstdint>

namespace sprk::aarch64
{
    uint32_t encode_mov_reg(int rd, int rm, bool is64bit);
    uint32_t encode_mvn_reg(int rd, int rm, uint32_t shift_type, uint32_t shift_amount, bool is64bit);
    uint32_t encode_mov_imm(int rd, uint64_t imm, bool is64bit);
    uint32_t encode_movz(int rd, uint16_t imm16, uint32_t shift, bool is64bit);
    uint32_t encode_movn(int rd, uint16_t imm16, uint32_t shift, bool is64bit);
    uint32_t encode_movk(int rd, uint16_t imm16, uint32_t shift, bool is64bit);
}