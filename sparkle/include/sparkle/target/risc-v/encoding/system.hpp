#pragma once

#include <cstdint>

namespace sprk::riscv
{
    /* Memory Fence */
    uint32_t encode_fence(int pred, int succ);

    /* Instruction Fence */
    uint32_t encode_fence_i();

    /* ENV Call */
    uint32_t encode_ecall();

    /* ENV Break */
    uint32_t encode_ebreak();

    /* CSR Read and Write */
    uint32_t encode_csrrw(int rd, int rs1, int csr);

    /* CSR Read and Set */
    uint32_t encode_csrrs(int rd, int rs1, int csr);

    /* CSR Read and Clear */
    uint32_t encode_csrrc(int rd, int rs1, int csr);

    /* CSR Read and Write Immediate */
    uint32_t encode_csrrwi(int rd, int zimm, int csr);

    /* CSR Read and Set Immediate */
    uint32_t encode_csrrsi(int rd, int zimm, int csr);

    /* CSR Read and Clear Immediate */
    uint32_t encode_csrrci(int rd, int zimm, int csr);
}
