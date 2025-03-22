#pragma once

#include <cstdint>

namespace sprk::aarch64
{
    uint32_t encode_b(int64_t offset);
    uint32_t encode_bl(int64_t offset);
    uint32_t encode_b_cond(uint32_t cond, int64_t offset);
    uint32_t encode_cbz(int rd, int64_t offset, bool is64bit);
    uint32_t encode_cbnz(int rd, int64_t offset, bool is64bit);
    uint32_t encode_tbz(int rt, uint32_t bit_pos, int64_t offset);
    uint32_t encode_tbnz(int rt, uint32_t bit_pos, int64_t offset);
}