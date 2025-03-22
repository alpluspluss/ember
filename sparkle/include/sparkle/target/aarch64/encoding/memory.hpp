#pragma once

#include <cstdint>

namespace sprk::aarch64
{
    uint32_t encode_ldr_imm(int rt, int rn, int imm12, uint32_t size);
    uint32_t encode_str_imm(int rt, int rn, int imm12, uint32_t size);
    uint32_t encode_ldr_reg(int rt, int rn, int rm, uint32_t option, bool extend_scale, uint32_t size);
    uint32_t encode_ldrsw_imm(int rt, int rn, int imm12);
    uint32_t encode_str_reg(int rt, int rn, int rm, uint32_t option, bool extend_scale, uint32_t size);
    uint32_t encode_ldur(int rt, int rn, int offset, uint32_t size);
    uint32_t encode_stur(int rt, int rn, int offset, uint32_t size);
    uint32_t encode_ldp(int rt1, int rt2, int rn, int imm7, uint32_t size, uint32_t option);
    uint32_t encode_stp(int rt1, int rt2, int rn, int imm7, uint32_t size, uint32_t option);
}