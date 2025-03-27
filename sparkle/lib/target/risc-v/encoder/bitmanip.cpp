#include <sparkle/target/risc-v/encoding/bitmanip.hpp>

namespace sprk::riscv
{
    uint32_t encode_clz(int rd, int rs1)
    {
        return (0b0110000 << 25) |  /* funct7 */
            (0b00000 << 20) |    /* rs2 = 0 */
            (rs1 << 15) |        /* rs1 */
            (0b001 << 12) |      /* funct3 */
            (rd << 7) |          /* rd */
            0b0010011;           /* opcode (OP-IMM) */
    }

    uint32_t encode_ctz(int rd, int rs1)
    {
        return (0b0110000 << 25) |  /* funct7 */
            (0b00001 << 20) |    /* rs2 = 1 */
            (rs1 << 15) |        /* rs1 */
            (0b001 << 12) |      /* funct3 */
            (rd << 7) |          /* rd */
            0b0010011;           /* opcode (OP-IMM) */
    }

    uint32_t encode_cpop(int rd, int rs1)
    {
        return (0b0110000 << 25) |  /* funct7 */
            (0b00010 << 20) |    /* rs2 = 2 */
            (rs1 << 15) |        /* rs1 */
            (0b001 << 12) |      /* funct3 */
            (rd << 7) |          /* rd */
            0b0010011;           /* opcode (OP-IMM) */
    }

    uint32_t encode_andn(int rd, int rs1, int rs2)
    {
        return (0b0100100 << 25) |  /* funct7 */
            (rs2 << 20) |        /* rs2 */
            (rs1 << 15) |        /* rs1 */
            (0b111 << 12) |      /* funct3 */
            (rd << 7) |          /* rd */
            0b0110011;           /* opcode (OP) */
    }

    uint32_t encode_orn(int rd, int rs1, int rs2)
    {
        return (0b0100100 << 25) |  /* funct7 */
            (rs2 << 20) |        /* rs2 */
            (rs1 << 15) |        /* rs1 */
            (0b110 << 12) |      /* funct3 */
            (rd << 7) |          /* rd */
            0b0110011;           /* opcode (OP) */
    }

    uint32_t encode_xnor(int rd, int rs1, int rs2)
    {
        return (0b0100100 << 25) |  /* funct7 */
            (rs2 << 20) |        /* rs2 */
            (rs1 << 15) |        /* rs1 */
            (0b100 << 12) |      /* funct3 */
            (rd << 7) |          /* rd */
            0b0110011;           /* opcode (OP) */
    }

    uint32_t encode_bext(int rd, int rs1, int rs2)
    {
        return (0b0100100 << 25) |  /* funct7 */
               (rs2 << 20) |        /* rs2 */
               (rs1 << 15) |        /* rs1 */
               (0b101 << 12) |      /* funct3 */
               (rd << 7) |          /* rd */
               0b0110011;           /* opcode (OP) */
    }

    uint32_t encode_bclr(int rd, int rs1, int rs2)
    {
        return (0b0100100 << 25) |  /* funct7 */
               (rs2 << 20) |        /* rs2 */
               (rs1 << 15) |        /* rs1 */
               (0b001 << 12) |      /* funct3 */
               (rd << 7) |          /* rd */
               0b0110011;           /* opcode (OP) */
    }

    uint32_t encode_bset(int rd, int rs1, int rs2)
    {
        return (0b0100100 << 25) |  /* funct7 */
               (rs2 << 20) |        /* rs2 */
               (rs1 << 15) |        /* rs1 */
               (0b001 << 12) |      /* funct3 */
               (rd << 7) |          /* rd */
               0b0110011;           /* opcode (OP) */
    }

    uint32_t encode_binv(int rd, int rs1, int rs2)
    {
        return (0b0100100 << 25) |  /* funct7 */
               (rs2 << 20) |        /* rs2 */
               (rs1 << 15) |        /* rs1 */
               (0b001 << 12) |      /* funct3 */
               (rd << 7) |          /* rd */
               0b0110011;           /* opcode (OP) */
    }
}