#pragma once

#include <cstdint>

namespace sprk::riscv
{
    uint32_t encode_fence(int pred, int succ)
    {
        /* pred, succ are each 4-bit fields specifying predecessors and successors */
        /* for FENCE operations e.g I/O, memory read, memory write etc. */
        int fm = 0;  /* FENCE mode, 0 for regular memory model */
        
        return (fm << 28) |         /* fm */
               (pred << 24) |       /* pred */
               (succ << 20) |       /* succ */
               (0 << 15) |          /* rs1 = 0 */
               (0b000 << 12) |      /* funct3 */
               (0 << 7) |           /* rd = 0 */
               0b0001111;           /* opcode */
    }

    uint32_t encode_fence_i()
    {
        return (0 << 20) |         /* imm[11:0] = 0 */
               (0 << 15) |         /* rs1 = 0 */
               (0b001 << 12) |     /* funct3 */
               (0 << 7) |          /* rd = 0 */
               0b0001111;          /* opcode */
    }

    uint32_t encode_ecall()
    {
        return (0 << 20) |         /* imm[11:0] = 0 */
               (0 << 15) |         /* rs1 = 0 */
               (0b000 << 12) |     /* funct3 */
               (0 << 7) |          /* rd = 0 */
               0b1110011;          /* opcode */
    }

    uint32_t encode_ebreak()
    {
        return (1 << 20) |         /* imm[11:0] = 1 */
               (0 << 15) |         /* rs1 = 0 */
               (0b000 << 12) |     /* funct3 */
               (0 << 7) |          /* rd = 0 */
               0b1110011;          /* opcode */
    }

    uint32_t encode_csrrw(int rd, int rs1, int csr)
    {
        /* CSR is a 12-bit address */
        int csr12 = csr & 0xFFF;
        
        return (csr12 << 20) |     /* csr */
               (rs1 << 15) |       /* rs1 */
               (0b001 << 12) |     /* funct3 */
               (rd << 7) |         /* rd */
               0b1110011;          /* opcode */
    }

    uint32_t encode_csrrs(int rd, int rs1, int csr)
    {
        /* CSR is a 12-bit address */
        int csr12 = csr & 0xFFF;
        
        return (csr12 << 20) |     /* csr */
               (rs1 << 15) |       /* rs1 */
               (0b010 << 12) |     /* funct3 */
               (rd << 7) |         /* rd */
               0b1110011;          /* opcode */
    }

    uint32_t encode_csrrc(int rd, int rs1, int csr)
    {
        /* CSR is a 12-bit address */
        int csr12 = csr & 0xFFF;
        
        return (csr12 << 20) |     /* csr */
               (rs1 << 15) |       /* rs1 */
               (0b011 << 12) |     /* funct3 */
               (rd << 7) |         /* rd */
               0b1110011;          /* opcode */
    }

    uint32_t encode_csrrwi(int rd, int zimm, int csr)
    {
        /* CSR is a 12-bit address */
        /* zimm is a 5-bit immeditea */
        int csr12 = csr & 0xFFF;
        int zimm5 = zimm & 0x1F;
        
        return (csr12 << 20) |     /* csr */
               (zimm5 << 15) |     /* zimm */
               (0b101 << 12) |     /* funct3 */
               (rd << 7) |         /* rd */
               0b1110011;          /* opcode */
    }

    uint32_t encode_csrrsi(int rd, int zimm, int csr)
    {
        /* CSR is a 12-bit address */
        /* zimm is a 5-bit immeditea */
        int csr12 = csr & 0xFFF;
        int zimm5 = zimm & 0x1F;
        
        return (csr12 << 20) |     /* csr */
               (zimm5 << 15) |     /* zimm */
               (0b110 << 12) |     /* funct3 */
               (rd << 7) |         /* rd */
               0b1110011;          /* opcode */
    }

    uint32_t encode_csrrci(int rd, int zimm, int csr)
    {
        /* CSR is a 12-bit address */
        /* zimm is a 5-bit immeditea */
        int csr12 = csr & 0xFFF;
        int zimm5 = zimm & 0x1F;
        
        return (csr12 << 20) |     /* csr */
               (zimm5 << 15) |     /* zimm */
               (0b111 << 12) |     /* funct3 */
               (rd << 7) |         /* rd */
               0b1110011;          /* opcode */
    }
}
