#pragma once

#include <cstdint>

namespace sprk::aarch64
{
    uint32_t encode_and_imm(int rd, int rn, uint64_t imm, bool is64bit, bool setflags);
    uint32_t encode_orr_imm(int rd, int rn, uint64_t imm, bool is64bit);
    uint32_t encode_eor_imm(int rd, int rn, uint64_t imm, bool is64bit);
    uint32_t encode_and_reg(int rd, int rn, int rm, uint32_t shift_type, uint32_t shift_amount, bool is64bit, bool setflags);
    uint32_t encode_bic_reg(int rd, int rn, int rm, uint32_t shift_type, uint32_t shift_amount, bool is64bit, bool setflags);
    uint32_t encode_orr_reg(int rd, int rn, int rm, uint32_t shift_type, uint32_t shift_amount, bool is64bit);
    uint32_t encode_orn_reg(int rd, int rn, int rm, uint32_t shift_type, uint32_t shift_amount, bool is64bit);
    uint32_t encode_eor_reg(int rd, int rn, int rm, uint32_t shift_type, uint32_t shift_amount, bool is64bit);
    uint32_t encode_eon_reg(int rd, int rn, int rm, uint32_t shift_type, uint32_t shift_amount, bool is64bit);
}