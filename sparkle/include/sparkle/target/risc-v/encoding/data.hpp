#pragma once

#include <cstdint>

namespace sprk::riscv
{
    /* Move register to register */
    /* note: this is a pseudo-instruction that expands to ADDI rd, rs, 0 */
    uint32_t encode_mv(int rd, int rs1);

    /* Load Upper Immediate */
    uint32_t encode_lui(int rd, int imm);

    /* Add Upper Immediate to PC */
    uint32_t encode_auipc(int rd, int imm);

    /* Jump and Link */
    uint32_t encode_jal(int rd, int offset);

    /* Jump and Link Register */
    uint32_t encode_jalr(int rd, int rs1, int offset);
}
