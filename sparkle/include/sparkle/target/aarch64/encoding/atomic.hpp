#pragma once

#include <cstdint>

namespace sprk::aarch64
{
    uint32_t encode_ldxr(int rt, int rn, uint32_t size);
    uint32_t encode_stxr(int rs, int rt, int rn, uint32_t size);
    uint32_t encode_ldaxr(int rt, int rn, uint32_t size);
    uint32_t encode_stlxr(int rs, int rt, int rn, uint32_t size);
    uint32_t encode_cas(int rs, int rt, int rn, uint32_t size, bool acquire, bool release);
    uint32_t encode_ldadd(int rs, int rt, int rn, uint32_t size, bool acquire, bool release);
    uint32_t encode_swp(int rs, int rt, int rn, uint32_t size, bool acquire, bool release);
}