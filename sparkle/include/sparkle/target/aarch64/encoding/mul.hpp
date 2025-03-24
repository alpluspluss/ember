#pragma once

#include <cstdint>

namespace sprk::aarch64
{
    uint32_t encode_mul(const int rd, const int rn, const int rm, const bool is64bit);

    uint32_t encode_madd(const int rd, const int rn, const int rm, const int ra, const bool is64bit);

    uint32_t encode_msub(const int rd, const int rn, const int rm, const int ra, const bool is64bit);

    uint32_t encode_mneg(const int rd, const int rn, const int rm, const bool is64bit);

    uint32_t encode_smaddl(const int rd, const int rn, const int rm, const int ra);

    uint32_t encode_smsubl(const int rd, const int rn, const int rm, const int ra);

    uint32_t encode_smull(const int rd, const int rn, const int rm);

    uint32_t encode_smnegl(const int rd, const int rn, const int rm);

    uint32_t encode_umaddl(const int rd, const int rn, const int rm, const int ra);

    uint32_t encode_umsubl(const int rd, const int rn, const int rm, const int ra);

    uint32_t encode_umull(const int rd, const int rn, const int rm);

    uint32_t encode_umnegl(const int rd, const int rn, const int rm);
}