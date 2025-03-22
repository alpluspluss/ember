#pragma once

#include <cstdint>

namespace sprk::aarch64
{
    uint32_t encode_fcmp(int rn, int rm, uint32_t size);
    uint32_t encode_fcmp_zero(int rn, uint32_t size);
    uint32_t encode_fmov_imm(int rd, float imm, uint32_t size);
    uint32_t encode_fcvt_to_int(int rd, int rn, bool is_signed, bool is64bit, uint32_t rounding, uint32_t size);
    uint32_t encode_fcvt_from_int(int rd, int rn, bool is_signed, bool is64bit, uint32_t size);
    uint32_t encode_fabs(int rd, int rn, uint32_t size);
    uint32_t encode_fneg(int rd, int rn, uint32_t size);
    uint32_t encode_fsqrt(int rd, int rn, uint32_t size);
    uint32_t encode_fadd(int rd, int rn, int rm, uint32_t size);
    uint32_t encode_fsub(int rd, int rn, int rm, uint32_t size);
    uint32_t encode_fmul(int rd, int rn, int rm, uint32_t size);
    uint32_t encode_fdiv(int rd, int rn, int rm, uint32_t size);
}