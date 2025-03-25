#pragma once

#include <cstdint>

namespace sprk::riscv
{
    /* Load-Reserved Word */
    uint32_t encode_lr_w(int rd, int rs1, bool aq, bool rl);

    /* Store-Conditional Word */
    uint32_t encode_sc_w(int rd, int rs1, int rs2, bool aq, bool rl);

    /* Atomic Swap Word */
    uint32_t encode_amoswap_w(int rd, int rs1, int rs2, bool aq, bool rl);

    /* Atomic Add Word */
    uint32_t encode_amoadd_w(int rd, int rs1, int rs2, bool aq, bool rl);

    /* Atomic XOR Word */
    uint32_t encode_amoxor_w(int rd, int rs1, int rs2, bool aq, bool rl);

    /* Atomic AND Word */
    uint32_t encode_amoand_w(int rd, int rs1, int rs2, bool aq, bool rl);

    /* Atomic OR Word */
    uint32_t encode_amoor_w(int rd, int rs1, int rs2, bool aq, bool rl);

    /* Atomic Minimum Word */
    uint32_t encode_amomin_w(int rd, int rs1, int rs2, bool aq, bool rl);

    /* Atomic Maximum Word */
    uint32_t encode_amomax_w(int rd, int rs1, int rs2, bool aq, bool rl);
}
