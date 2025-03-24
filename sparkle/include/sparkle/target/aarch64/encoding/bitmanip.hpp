#pragma once

#include <cstdint>

namespace sprk::aarch64
{
    uint32_t encode_rbit(const int rd, const int rn, const bool is64bit);

    uint32_t encode_rev16(const int rd, const int rn, const bool is64bit);

    uint32_t encode_rev32(const int rd, const int rn, const bool is64bit);

    uint32_t encode_rev(const int rd, const int rn, const bool is64bit);

    /* commented out because we couldn't verify if the encoding is legitimate */
    // uint32_t encode_clz(const int rd, const int rn, const bool is64bit);

    // uint32_t encode_cls(const int rd, const int rn, const bool is64bit);

    // /* for newer ARM revisions */
    // uint32_t encode_ctz(const int rd, const int rn, const bool is64bit);

    // /* ARMv8.3-A+ extensions */
    // uint32_t encode_cnt(const int rd, const int rn, const bool is64bit);
}