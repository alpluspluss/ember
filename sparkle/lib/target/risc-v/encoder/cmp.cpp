#include <sparkle/target/risc-v/encoding/cmp.hpp>

namespace sprk::riscv
{
    uint32_t encode_slt(int rd, int rs1, int rs2)
    {
        return (0b0000000 << 25) |  /* funct7 */
               (rs2 << 20) |        /* rs2 */
               (rs1 << 15) |        /* rs1 */
               (0b010 << 12) |      /* funct3 */
               (rd << 7) |          /* rd */
               0b0110011;           /* opcode (OP) */
    }

    uint32_t encode_sltu(int rd, int rs1, int rs2)
    {
        return (0b0000000 << 25) |  /* funct7 */
               (rs2 << 20) |        /* rs2 */
               (rs1 << 15) |        /* rs1 */
               (0b011 << 12) |      /* funct3 */
               (rd << 7) |          /* rd */
               0b0110011;           /* opcode (OP) */
    }

    uint32_t encode_slti(int rd, int rs1, int imm)
    {
        /* imm is 12 bits, sign-extended */
        int imm12 = imm & 0xFFF;
        
        return (imm12 << 20) |      /* imm[11:0] */
               (rs1 << 15) |        /* rs1 */
               (0b010 << 12) |      /* funct3 */
               (rd << 7) |          /* rd */
               0b0010011;           /* opcode (OP-IMM) */
    }

    uint32_t encode_sltiu(int rd, int rs1, int imm)
    {
        /* imm is 12 bits, sign-extended */
        int imm12 = imm & 0xFFF;
        
        return (imm12 << 20) |      /* imm[11:0] */
               (rs1 << 15) |        /* rs1 */
               (0b011 << 12) |      /* funct3 */
               (rd << 7) |          /* rd */
               0b0010011;           /* opcode (OP-IMM) */
    }
}