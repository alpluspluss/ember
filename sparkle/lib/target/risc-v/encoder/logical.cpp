#include <sparkle/target/risc-v/encoding/logical.hpp>

namespace sprk::riscv
{
    uint32_t encode_band(int rd, int rs1, int rs2)
    {
        return (0b0000000 << 25) |  /* funct7 */
               (rs2 << 20) |        /* rs2 */
               (rs1 << 15) |        /* rs1 */
               (0b111 << 12) |      /* funct3 */
               (rd << 7) |          /* rd */
               0b0110011;           /* opcode */
    }

    uint32_t encode_bor(int rd, int rs1, int rs2)
    {
        return (0b0000000 << 25) |  /* funct7 */
               (rs2 << 20) |        /* rs2 */
               (rs1 << 15) |        /* rs1 */
               (0b110 << 12) |      /* funct3 */
               (rd << 7) |          /* rd */
               0b0110011;           /* opcode */
    }

    uint32_t encode_bxor(int rd, int rs1, int rs2)
    {
        return (0b0000000 << 25) |  /* funct7 */
               (rs2 << 20) |        /* rs2 */
               (rs1 << 15) |        /* rs1 */
               (0b100 << 12) |      /* funct3 */
               (rd << 7) |          /* rd */
               0b0110011;           /* opcode */
    }

    uint32_t encode_bandi(int rd, int rs1, int imm)
    {
        /* imm is 12 bits, sign-extended */
        int imm12 = imm & 0xFFF;
        
        return (imm12 << 20) |      /* imm[11:0] */
               (rs1 << 15) |        /* rs1 */
               (0b111 << 12) |      /* funct3 */
               (rd << 7) |          /* rd */
               0b0010011;           /* opcode */
    }

    uint32_t encode_bori(int rd, int rs1, int imm)
    {
        /* imm is 12 bits, sign-extended */
        int imm12 = imm & 0xFFF;
        
        return (imm12 << 20) |      /* imm[11:0] */
               (rs1 << 15) |        /* rs1 */
               (0b110 << 12) |      /* funct3 */
               (rd << 7) |          /* rd */
               0b0010011;           /* opcode */
    }

    uint32_t encode_bxori(int rd, int rs1, int imm)
    {
        /* imm is 12 bits, sign-extended */
        int imm12 = imm & 0xFFF;
        
        return (imm12 << 20) |      /* imm[11:0] */
               (rs1 << 15) |        /* rs1 */
               (0b100 << 12) |      /* funct3 */
               (rd << 7) |          /* rd */
               0b0010011;           /* opcode */
    }
}