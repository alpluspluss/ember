#pragma once

#include <cstdint>

namespace sprk::riscv
{
    uint32_t encode_add(int rd, int rs1, int rs2);

    uint32_t encode_sub(int rd, int rs1, int rs2);
    
    uint32_t encode_addi(int rd, int rs1, int imm);

    /* RV64I only */
    uint32_t encode_addiw(int rd, int rs1, int imm);

    /* RV64I only */
    uint32_t encode_addw(int rd, int rs1, int rs2);

    /* RV64I only */
    uint32_t encode_subw(int rd, int rs1, int rs2);
}
