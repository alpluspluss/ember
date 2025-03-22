#pragma once

#include <cstdint>

namespace sprk::aarch64
{
    uint32_t encode_bfm(int rd, int rn, uint32_t immr, uint32_t imms, bool is64bit, uint32_t opc);
    uint32_t encode_bfi(int rd, int rn, uint32_t lsb, uint32_t width, bool is64bit);
    uint32_t encode_bfxil(int rd, int rn, uint32_t lsb, uint32_t width, bool is64bit);
    uint32_t encode_sbfx(int rd, int rn, uint32_t lsb, uint32_t width, bool is64bit);
    uint32_t encode_ubfx(int rd, int rn, uint32_t lsb, uint32_t width, bool is64bit);
    uint32_t encode_extr(int rd, int rn, int rm, uint32_t lsb, bool is64bit);
    uint32_t encode_asr_imm_alias(int rd, int rn, uint32_t shift, bool is64bit);
}