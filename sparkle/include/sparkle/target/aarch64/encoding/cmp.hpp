#pragma once

#include <cstdint>

namespace sprk::aarch64
{
    uint32_t encode_cmp_imm(int rn, int imm12, bool is64bit);
    uint32_t encode_cmp_reg(int rn, int rm, uint32_t shift_type, uint32_t shift_amount, bool is64bit);
    uint32_t encode_tst_reg(int rn, int rm, uint32_t shift_type, uint32_t shift_amount, bool is64bit);
}