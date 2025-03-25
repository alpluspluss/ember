#include <sparkle/target/risc-v/encoding/arith.hpp>

namespace sprk::riscv
{
    uint32_t encode_add(int rd, int rs1, int rs2)
    {
        return (0b0000000 << 25) |  /* funct7 */
               ((rs2 & 0x1F) << 20) |  /* rs2 */
               ((rs1 & 0x1F) << 15) |  /* rs1 */
               (0b000 << 12) |      /* funct3 */
               ((rd & 0x1F) << 7) |  /* rd */
               0b0110011;           /* opcode */
    }

    uint32_t encode_sub(int rd, int rs1, int rs2)
    {
        return (0b0100000 << 25) |  /* funct7 */
               ((rs2 & 0x1F) << 20) |  /* rs2 */
               ((rs1 & 0x1F) << 15) |  /* rs1 */
               (0b000 << 12) |      /* funct3 */
               ((rd & 0x1F) << 7) |  /* rd */
               0b0110011;           /* opcode */
    }

    uint32_t encode_addi(int rd, int rs1, int imm)
    {
        /* imm is 12 bits, sign-extended */
        int imm12 = imm & 0xFFF;
        
        return (imm12 << 20) |      /* imm[11:0] */
               ((rs1 & 0x1F) << 15) |  /* rs1 */
               (0b000 << 12) |      /* funct3 */
               ((rd & 0x1F) << 7) |  /* rd */
               0b0010011;           /* opcode */
    }

    uint32_t encode_addiw(int rd, int rs1, int imm)
    {
        /* imm is 12 bits, sign-extended */
        int imm12 = imm & 0xFFF;
        
        return (imm12 << 20) |      /* imm[11:0] */
               ((rs1 & 0x1F) << 15) |  /* rs1 */
               (0b000 << 12) |      /* funct3 */
               ((rd & 0x1F) << 7) |  /* rd */
               0b0011011;           /* opcode */
    }

    uint32_t encode_addw(int rd, int rs1, int rs2)
    {
        return (0b0000000 << 25) |  /* funct7 */
               ((rs2 & 0x1F) << 20) |  /* rs2 */
               ((rs1 & 0x1F) << 15) |  /* rs1 */
               (0b000 << 12) |      /* funct3 */
               ((rd & 0x1F) << 7) |  /* rd */
               0b0111011;           /* opcode */
    }

    uint32_t encode_subw(int rd, int rs1, int rs2)
    {
        return (0b0100000 << 25) |  /* funct7 */
               ((rs2 & 0x1F) << 20) |  /* rs2 */
               ((rs1 & 0x1F) << 15) |  /* rs1 */
               (0b000 << 12) |      /* funct3 */
               ((rd & 0x1F) << 7) |  /* rd */
               0b0111011;           /* opcode */
    }
}