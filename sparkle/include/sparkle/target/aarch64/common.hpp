#pragma once

#include <cstdint>

namespace sprk::aarch64
{
    bool encode_logical_imm(uint64_t imm, bool is64bit, uint32_t* N, uint32_t* immr, uint32_t* imms);
}

#define SHIFT_LSL 0
#define SHIFT_LSR 1
#define SHIFT_ASR 2
#define SHIFT_ROR 3

#define COND_EQ  0  /* equal */
#define COND_NE  1  /* not Equal */
#define COND_CS  2  /* carry set (or HS: higher or same) */
#define COND_CC  3  /* carry clear (or LO: lower) */
#define COND_MI  4  /* minus/negative */
#define COND_PL  5  /* plus/positive or Zero */
#define COND_VS  6  /* overflow */
#define COND_VC  7  /* no overflow */
#define COND_HI  8  /* higher */
#define COND_LS  9  /* lower or same */
#define COND_GE  10 /* greater or equal */
#define COND_LT  11 /* less than */
#define COND_GT  12 /* greater than */
#define COND_LE  13 /* less or equal */
#define COND_AL  14 /* always */
#define COND_NV  15 /* never (reserved) */

#define SIZE_BYTE      0
#define SIZE_HALFWORD  1
#define SIZE_WORD      2
#define SIZE_DOUBLEWORD 3

#define SIMD_8B   0
#define SIMD_16B  1
#define SIMD_4H   2
#define SIMD_8H   3
#define SIMD_2S   4
#define SIMD_4S   5
#define SIMD_2D   6

#define FP16 0 
#define FP32 1
#define FP64 2