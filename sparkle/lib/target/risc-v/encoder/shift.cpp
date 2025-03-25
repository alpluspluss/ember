#include <sparkle/target/risc-v/encoding/shift.hpp>

namespace sprk::riscv
{
    uint32_t encode_sll(int rd, int rs1, int rs2)
    {
        return (0b0000000 << 25) |       /* funct7 */
               ((rs2 & 0x1F) << 20) |    /* rs2 */
               ((rs1 & 0x1F) << 15) |    /* rs1 */
               (0b001 << 12) |           /* funct3 */
               ((rd & 0x1F) << 7) |      /* rd */
               0b0110011;                /* opcode */
    }

    uint32_t encode_srl(int rd, int rs1, int rs2)
    {
        return (0b0000000 << 25) |       /* funct7 */
               ((rs2 & 0x1F) << 20) |    /* rs2 */
               ((rs1 & 0x1F) << 15) |    /* rs1 */
               (0b101 << 12) |           /* funct3 */
               ((rd & 0x1F) << 7) |      /* rd */
               0b0110011;                /* opcode */
    }

    uint32_t encode_sra(int rd, int rs1, int rs2)
    {
        return (0b0100000 << 25) |       /* funct7 */
               ((rs2 & 0x1F) << 20) |    /* rs2 */
               ((rs1 & 0x1F) << 15) |    /* rs1 */
               (0b101 << 12) |           /* funct3 */
               ((rd & 0x1F) << 7) |      /* rd */
               0b0110011;                /* opcode */
    }

    uint32_t encode_slli(int rd, int rs1, int shamt)
    {
        /* for RV32I, shamt is 5 bits (0-31) */
        int shift = shamt & 0x1F;
        
        return (0b0000000 << 25) |       /* funct7 */
               (shift << 20) |           /* shamt */
               ((rs1 & 0x1F) << 15) |    /* rs1 */
               (0b001 << 12) |           /* funct3 */
               ((rd & 0x1F) << 7) |      /* rd */
               0b0010011;                /* opcode */
    }

    uint32_t encode_srli(int rd, int rs1, int shamt)
    {
        /* for RV32I, shamt is 5 bits (0-31) */
        int shift = shamt & 0x1F;
        
        return (0b0000000 << 25) |       /* funct7 */
               (shift << 20) |           /* shamt */
               ((rs1 & 0x1F) << 15) |    /* rs1 */
               (0b101 << 12) |           /* funct3 */
               ((rd & 0x1F) << 7) |      /* rd */
               0b0010011;                /* opcode */
    }

    uint32_t encode_srai(int rd, int rs1, int shamt)
    {
        // For RV32I, shamt is 5 bits (0-31)
        int shift = shamt & 0x1F;
        
        return (0b0100000 << 25) |       /* funct7 */
               (shift << 20) |           /* shamt */
               ((rs1 & 0x1F) << 15) |    /* rs1 */
               (0b101 << 12) |           /* funct3 */
               ((rd & 0x1F) << 7) |      /* rd */
               0b0010011;                /* opcode */
    }

    uint32_t encode_sllw(int rd, int rs1, int rs2, bool is64bit)
    {
        return (0b0000000 << 25) |       /* funct7 */
               ((rs2 & 0x1F) << 20) |    /* rs2 */
               ((rs1 & 0x1F) << 15) |    /* rs1 */
               (0b001 << 12) |           /* funct3 */
               ((rd & 0x1F) << 7) |      /* rd */
               0b0111011;                /* opcode - RV64I "W" opcode */
    }

    uint32_t encode_srlw(int rd, int rs1, int rs2, bool is64bit)
    {
        return (0b0000000 << 25) |       /* funct7 */
               ((rs2 & 0x1F) << 20) |    /* rs2 */
               ((rs1 & 0x1F) << 15) |    /* rs1 */
               (0b101 << 12) |           /* funct3 */
               ((rd & 0x1F) << 7) |      /* rd */
               0b0111011;                /* opcode - RV64I "W" opcode */
    }

    uint32_t encode_sraw(int rd, int rs1, int rs2, bool is64bit)
    {
        return (0b0100000 << 25) |       /* funct7 */
               ((rs2 & 0x1F) << 20) |    /* rs2 */
               ((rs1 & 0x1F) << 15) |    /* rs1 */
               (0b101 << 12) |           /* funct3 */
               ((rd & 0x1F) << 7) |      /* rd */
               0b0111011;                /* opcode - RV64I "W" opcode */
    }
}