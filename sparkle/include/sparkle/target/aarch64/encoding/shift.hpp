#pragma once

#include <cstdint>

namespace sprk::aarch64
{
    uint32_t encode_lslv(int rd, int rn, int rm, bool is64bit);
    uint32_t encode_lsrv(int rd, int rn, int rm, bool is64bit);
    uint32_t encode_asrv(int rd, int rn, int rm, bool is64bit);
    uint32_t encode_rorv(int rd, int rn, int rm, bool is64bit);
    uint32_t encode_lsl_imm(int rd, int rn, uint32_t shift, bool is64bit);
    uint32_t encode_lsr_imm(int rd, int rn, uint32_t shift, bool is64bit);
    uint32_t encode_asr_imm(int rd, int rn, uint32_t shift, bool is64bit);
    uint32_t encode_ror_imm(int rd, int rn, uint32_t shift, bool is64bit);
}