#include <sparkle/target/risc-v/encoding/mul.hpp>

namespace sprk::riscv
{
    uint32_t encode_mul(int rd, int rs1, int rs2)
    {
        return (0b0000001 << 25) |  /* funct7 */
               (rs2 << 20) |        /* rs2 */
               (rs1 << 15) |        /* rs1 */
               (0b000 << 12) |      /* funct3 */
               (rd << 7) |          /* rd */
               0b0110011;           /* opcode */
    }

    uint32_t encode_mulh(int rd, int rs1, int rs2)
    {
        return (0b0000001 << 25) |  /* funct7 */
               (rs2 << 20) |        /* rs2 */
               (rs1 << 15) |        /* rs1 */
               (0b001 << 12) |      /* funct3 */
               (rd << 7) |          /* rd */
               0b0110011;           /* opcode */
    }

    uint32_t encode_mulhsu(int rd, int rs1, int rs2)
    {
        return (0b0000001 << 25) |  /* funct7 */
               (rs2 << 20) |        /* rs2 */
               (rs1 << 15) |        /* rs1 */
               (0b010 << 12) |      /* funct3 */
               (rd << 7) |          /* rd */
               0b0110011;           /* opcode */
    }

    uint32_t encode_mulhu(int rd, int rs1, int rs2)
    {
        return (0b0000001 << 25) |  /* funct7 */
               (rs2 << 20) |        /* rs2 */
               (rs1 << 15) |        /* rs1 */
               (0b011 << 12) |      /* funct3 */
               (rd << 7) |          /* rd */
               0b0110011;           /* opcode */
    }

    uint32_t encode_div(int rd, int rs1, int rs2)
    {
        return (0b0000001 << 25) |  /* funct7 */
               (rs2 << 20) |        /* rs2 */
               (rs1 << 15) |        /* rs1 */
               (0b100 << 12) |      /* funct3 */
               (rd << 7) |          /* rd */
               0b0110011;           /* opcode */
    }

    uint32_t encode_divu(int rd, int rs1, int rs2)
    {
        return (0b0000001 << 25) |  /* funct7 */
               (rs2 << 20) |        /* rs2 */
               (rs1 << 15) |        /* rs1 */
               (0b101 << 12) |      /* funct3 */
               (rd << 7) |          /* rd */
               0b0110011;           /* opcode */
    }

    uint32_t encode_rem(int rd, int rs1, int rs2)
    {
        return (0b0000001 << 25) |  /* funct7 */
               (rs2 << 20) |        /* rs2 */
               (rs1 << 15) |        /* rs1 */
               (0b110 << 12) |      /* funct3 */
               (rd << 7) |          /* rd */
               0b0110011;           /* opcode */
    }

    uint32_t encode_remu(int rd, int rs1, int rs2)
    {
        return (0b0000001 << 25) |  /* funct7 */
               (rs2 << 20) |        /* rs2 */
               (rs1 << 15) |        /* rs1 */
               (0b111 << 12) |      /* funct3 */
               (rd << 7) |          /* rd */
               0b0110011;           /* opcode */
    }


    /* RV64 instrs */



    uint32_t encode_mulw(int rd, int rs1, int rs2)
    {
        return (0b0000001 << 25) |  /* funct7 */
               (rs2 << 20) |        /* rs2 */
               (rs1 << 15) |        /* rs1 */
               (0b000 << 12) |      /* funct3 */
               (rd << 7) |          /* rd */
               0b0111011;           /* opcode */
    }

    uint32_t encode_divw(int rd, int rs1, int rs2)
    {
        return (0b0000001 << 25) |  /* funct7 */
               (rs2 << 20) |        /* rs2 */
               (rs1 << 15) |        /* rs1 */
               (0b100 << 12) |      /* funct3 */
               (rd << 7) |          /* rd */
               0b0111011;           /* opcode */
    }

    uint32_t encode_divuw(int rd, int rs1, int rs2)
    {
        return (0b0000001 << 25) |  /* funct7 */
               (rs2 << 20) |        /* rs2 */
               (rs1 << 15) |        /* rs1 */
               (0b101 << 12) |      /* funct3 */
               (rd << 7) |          /* rd */
               0b0111011;           /* opcode */
    }

    uint32_t encode_remw(int rd, int rs1, int rs2)
    {
        return (0b0000001 << 25) |  /* funct7 */
               (rs2 << 20) |        /* rs2 */
               (rs1 << 15) |        /* rs1 */
               (0b110 << 12) |      /* funct3 */
               (rd << 7) |          /* rd */
               0b0111011;           /* opcode */
    }

    uint32_t encode_remuw(int rd, int rs1, int rs2)
    {
        return (0b0000001 << 25) |  /* funct7 */
               (rs2 << 20) |        /* rs2 */
               (rs1 << 15) |        /* rs1 */
               (0b111 << 12) |      /* funct3 */
               (rd << 7) |          /* rd */
               0b0111011;           /* opcode */
    }
}
