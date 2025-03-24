#pragma once

#include <cstdint>

namespace sprk::aarch64
{
    uint32_t encode_csel(const int rd, const int rn, const int rm, const uint32_t cond, const bool is64bit);

    uint32_t encode_csinc(const int rd, const int rn, const int rm, const uint32_t cond, const bool is64bit);

    uint32_t encode_csinv(const int rd, const int rn, const int rm, const uint32_t cond, const bool is64bit);
    
    uint32_t encode_csneg(const int rd, const int rn, const int rm, const uint32_t cond, const bool is64bit);

    uint32_t encode_cinc(const int rd, const int rn, const uint32_t cond, const bool is64bit);

    uint32_t encode_cinv(const int rd, const int rn, const uint32_t cond, const bool is64bit);

    uint32_t encode_cneg(const int rd, const int rn, const uint32_t cond, const bool is64bit);

    uint32_t encode_cset(const int rd, const uint32_t cond, const bool is64bit);

    uint32_t encode_csetm(const int rd, const uint32_t cond, const bool is64bit);
}