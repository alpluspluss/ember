#pragma once

#include <cstdint>

namespace sprk::aarch64
{
    uint32_t encode_movi(int rd, uint64_t imm, uint32_t arrangement);
    uint32_t encode_add_simd(int rd, int rn, int rm, uint32_t arrangement);
    uint32_t encode_sub_simd(int rd, int rn, int rm, uint32_t arrangement);
    uint32_t encode_mul_simd(int rd, int rn, int rm, uint32_t arrangement);
    uint32_t encode_and_simd(int rd, int rn, int rm, uint32_t arrangement);
    uint32_t encode_orr_simd(int rd, int rn, int rm, uint32_t arrangement);
    uint32_t encode_tbl(int rd, int rn, int rm, uint32_t len, bool is16b);
    uint32_t encode_dup_elem(int rd, int rn, uint32_t index, uint32_t arrangement);
    uint32_t encode_dup_gen(int rd, int rn, uint32_t arrangement);
    uint32_t encode_ins(int rd, int rn, uint32_t dst_index, uint32_t src_index, uint32_t size);
    uint32_t encode_umov(int rd, int rn, uint32_t index, uint32_t size);
}