#include <sparkle/target/risc-v/encoding/memory.hpp>

namespace sprk::riscv
{
    uint32_t encode_lb(int rd, int rs1, int offset)
    {
        /* imm is 12 bits, sign-extended */
        int imm12 = offset & 0xFFF;
        
        return (imm12 << 20) |      /* imm[11:0] */
               (rs1 << 15) |        /* rs1 */
               (0b000 << 12) |      /* funct3 */
               (rd << 7) |          /* rd */
               0b0000011;           /* opcode */
    }

    uint32_t encode_lh(int rd, int rs1, int offset)
    {
        /* imm is 12 bits, sign-extended */
        int imm12 = offset & 0xFFF;
        
        return (imm12 << 20) |      /* imm[11:0] */
               (rs1 << 15) |        /* rs1 */
               (0b001 << 12) |      /* funct3 */
               (rd << 7) |          /* rd */
               0b0000011;           /* opcode */
    }

    uint32_t encode_lw(int rd, int rs1, int offset)
    {
        /* imm is 12 bits, sign-extended */
        int imm12 = offset & 0xFFF;
        
        return (imm12 << 20) |      /* imm[11:0] */
               (rs1 << 15) |        /* rs1 */
               (0b010 << 12) |      /* funct3 */
               (rd << 7) |          /* rd */
               0b0000011;           /* opcode */
    }

    uint32_t encode_lbu(int rd, int rs1, int offset)
    {
        /* imm is 12 bits, sign-extended */
        int imm12 = offset & 0xFFF;
        
        return (imm12 << 20) |      /* imm[11:0] */
               (rs1 << 15) |        /* rs1 */
               (0b100 << 12) |      /* funct3 */
               (rd << 7) |          /* rd */
               0b0000011;           /* opcode */
    }

    uint32_t encode_lhu(int rd, int rs1, int offset)
    {
        /* imm is 12 bits, sign-extended */
        int imm12 = offset & 0xFFF;
        
        return (imm12 << 20) |      /* imm[11:0] */
               (rs1 << 15) |        /* rs1 */
               (0b101 << 12) |      /* funct3 */
               (rd << 7) |          /* rd */
               0b0000011;           /* opcode */
    }

    uint32_t encode_lwu(int rd, int rs1, int offset)
    {
        /* imm is 12 bits, sign-extended */
        int imm12 = offset & 0xFFF;
        
        return (imm12 << 20) |      /* imm[11:0] */
               (rs1 << 15) |        /* rs1 */
               (0b110 << 12) |      /* funct3 */
               (rd << 7) |          /* rd */
               0b0000011;           /* opcode */
    }

    uint32_t encode_ld(int rd, int rs1, int offset)
    {
        /* imm is 12 bits, sign-extended */
        int imm12 = offset & 0xFFF;
        
        return (imm12 << 20) |      /* imm[11:0] */
               (rs1 << 15) |        /* rs1 */
               (0b011 << 12) |      /* funct3 */
               (rd << 7) |          /* rd */
               0b0000011;           /* opcode */
    }

    uint32_t encode_sb(int rs2, int rs1, int offset)
    {
        /* imm is 12 bits, sign-extended */
        int imm12 = offset & 0xFFF;
        
        return ((imm12 >> 5) << 25) | /* imm[11:5] */
               (rs2 << 20) |          /* rs2 */
               (rs1 << 15) |          /* rs1 */
               (0b000 << 12) |        /* funct3 */
               ((imm12 & 0x1F) << 7) | /* imm[4:0] */
               0b0100011;             /* opcode */
    }

    uint32_t encode_sh(int rs2, int rs1, int offset)
    {
        /* imm is 12 bits, sign-extended */
        int imm12 = offset & 0xFFF;
        
        return ((imm12 >> 5) << 25) | /* imm[11:5] */
               (rs2 << 20) |          /* rs2 */
               (rs1 << 15) |          /* rs1 */
               (0b001 << 12) |        /* funct3 */
               ((imm12 & 0x1F) << 7) | /* imm[4:0] */
               0b0100011;             /* opcode */
    }

    uint32_t encode_sw(int rs2, int rs1, int offset)
    {
        /* imm is 12 bits, sign-extended */
        int imm12 = offset & 0xFFF;
        
        return ((imm12 >> 5) << 25) | /* imm[11:5] */
               (rs2 << 20) |          /* rs2 */
               (rs1 << 15) |          /* rs1 */
               (0b010 << 12) |        /* funct3 */
               ((imm12 & 0x1F) << 7) | /* imm[4:0] */
               0b0100011;             /* opcode */
    }

    uint32_t encode_sd(int rs2, int rs1, int offset)
    {
        /* imm is 12 bits, sign-extended */
        int imm12 = offset & 0xFFF;
        
        return ((imm12 >> 5) << 25) | /* imm[11:5] */
               (rs2 << 20) |          /* rs2 */
               (rs1 << 15) |          /* rs1 */
               (0b011 << 12) |        /* funct3 */
               ((imm12 & 0x1F) << 7) | /* imm[4:0] */
               0b0100011;             /* opcode */
    }
}