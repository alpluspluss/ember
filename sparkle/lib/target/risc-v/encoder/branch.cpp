#include <sparkle/target/risc-v/encoding/branch.hpp>

namespace sprk::riscv
{
    uint32_t encode_beq(int rs1, int rs2, int offset)
    {
        /* imm must be a PO2 */
        if (offset & 0x1) 
            offset &= ~1;
        if (offset < -4096 || offset > 4094) /* if out of range, set the offset to maximum */
            offset = (offset < 0) ? -4096 : 4094;
        
        /* extract bits to get the B-type immediate */
        int imm12 = (offset >> 1) & 0xFFF;
        int imm_12 = (imm12 >> 11) & 0x1;
        int imm_11 = (imm12 >> 10) & 0x1;
        int imm_10_5 = (imm12 >> 4) & 0x3F;
        int imm_4_1 = imm12 & 0xF;
        
        return (imm_12 << 31) |     /* imm[12] */
               (imm_10_5 << 25) |   /* imm[10:5] */
               (rs2 << 20) |        /* rs2 */
               (rs1 << 15) |        /* rs1 */
               (0b000 << 12) |      /* funct3 */
               (imm_4_1 << 8) |     /* imm[4:1] */
               (imm_11 << 7) |      /* imm[11] */
               0b1100011;           /* opcode */
    }

    uint32_t encode_bne(int rs1, int rs2, int offset)
    {
        /* imm must be a PO2 */
        if (offset & 0x1)
            offset &= ~1;
        if (offset < -4096 || offset > 4094)
            /* if out of range, set the offset to maximum */
            offset = (offset < 0) ? -4096 : 4094;
        
        /* extract bits to get the B-type immediate */
        int imm12 = (offset >> 1) & 0xFFF;
        int imm_12 = (imm12 >> 11) & 0x1;
        int imm_11 = (imm12 >> 10) & 0x1;
        int imm_10_5 = (imm12 >> 4) & 0x3F;
        int imm_4_1 = imm12 & 0xF;
        
        return (imm_12 << 31) |     /* imm[12] */
               (imm_10_5 << 25) |   /* imm[10:5] */
               (rs2 << 20) |        /* rs2 */
               (rs1 << 15) |        /* rs1 */
               (0b001 << 12) |      /* funct3 */
               (imm_4_1 << 8) |     /* imm[4:1] */
               (imm_11 << 7) |      /* imm[11] */
               0b1100011;           /* opcode */
    }

    uint32_t encode_blt(int rs1, int rs2, int offset)
    {
        /* imm must be a PO2 */
        if (offset & 0x1)
            // Error: offset not multiple of 2, but we'll handle it by clearing the bottom bit
            offset &= ~1;
        if (offset < -4096 || offset > 4094)
            /* if out of range, set the offset to maximum */
            offset = (offset < 0) ? -4096 : 4094;
        
        /* extract bits to get the B-type immediate */
        int imm12 = (offset >> 1) & 0xFFF;
        int imm_12 = (imm12 >> 11) & 0x1;
        int imm_11 = (imm12 >> 10) & 0x1;
        int imm_10_5 = (imm12 >> 4) & 0x3F;
        int imm_4_1 = imm12 & 0xF;
        
        return (imm_12 << 31) |     /* imm[12] */
               (imm_10_5 << 25) |   /* imm[10:5] */
               (rs2 << 20) |        /* rs2 */
               (rs1 << 15) |        /* rs1 */
               (0b100 << 12) |      /* funct3 */
               (imm_4_1 << 8) |     /* imm[4:1] */
               (imm_11 << 7) |      /* imm[11] */
               0b1100011;           /* opcode */
    }

    uint32_t encode_bge(int rs1, int rs2, int offset)
    {
        /* imm must be a PO2 */
        if (offset & 0x1)
            // Error: offset not multiple of 2, but we'll handle it by clearing the bottom bit
            offset &= ~1;
        if (offset < -4096 || offset > 4094)
            /* if out of range, set the offset to maximum */
            offset = (offset < 0) ? -4096 : 4094;
        
        /* extract bits to get the B-type immediate */
        int imm12 = (offset >> 1) & 0xFFF;
        int imm_12 = (imm12 >> 11) & 0x1;
        int imm_11 = (imm12 >> 10) & 0x1;
        int imm_10_5 = (imm12 >> 4) & 0x3F;
        int imm_4_1 = imm12 & 0xF;
        
        return (imm_12 << 31) |     /* imm[12] */
               (imm_10_5 << 25) |   /* imm[10:5] */
               (rs2 << 20) |        /* rs2 */
               (rs1 << 15) |        /* rs1 */
               (0b101 << 12) |      /* funct3 */
               (imm_4_1 << 8) |     /* imm[4:1] */
               (imm_11 << 7) |      /* imm[11] */
               0b1100011;           /* opcode */
    }

    uint32_t encode_bltu(int rs1, int rs2, int offset)
    {
        /* imm must be a PO2 */
        if (offset & 0x1)
            // Error: offset not multiple of 2, but we'll handle it by clearing the bottom bit
            offset &= ~1;
        if (offset < -4096 || offset > 4094)
            /* if out of range, set the offset to maximum */
            offset = (offset < 0) ? -4096 : 4094;
        
        /* extract bits to get the B-type immediate */
        int imm12 = (offset >> 1) & 0xFFF;
        int imm_12 = (imm12 >> 11) & 0x1;
        int imm_11 = (imm12 >> 10) & 0x1;
        int imm_10_5 = (imm12 >> 4) & 0x3F;
        int imm_4_1 = imm12 & 0xF;
        
        return (imm_12 << 31) |     /* imm[12] */
               (imm_10_5 << 25) |   /* imm[10:5] */
               (rs2 << 20) |        /* rs2 */
               (rs1 << 15) |        /* rs1 */
               (0b110 << 12) |      /* funct3 */
               (imm_4_1 << 8) |     /* imm[4:1] */
               (imm_11 << 7) |      /* imm[11] */
               0b1100011;           /* opcode */
    }

    uint32_t encode_bgeu(int rs1, int rs2, int offset)
    {
        /* imm must be a PO2 */
        if (offset & 0x1) 
            // Error: offset not multiple of 2, but we'll handle it by clearing the bottom bit
            offset &= ~1;
        if (offset < -4096 || offset > 4094) 
            /* if out of range, set the offset to maximum */
            offset = (offset < 0) ? -4096 : 4094;
        
        /* extract bits to get the B-type immediate */
        int imm12 = (offset >> 1) & 0xFFF;
        int imm_12 = (imm12 >> 11) & 0x1;
        int imm_11 = (imm12 >> 10) & 0x1;
        int imm_10_5 = (imm12 >> 4) & 0x3F;
        int imm_4_1 = imm12 & 0xF;
        
        return (imm_12 << 31) |     /* imm[12] */
               (imm_10_5 << 25) |   /* imm[10:5] */
               (rs2 << 20) |        /* rs2 */
               (rs1 << 15) |        /* rs1 */
               (0b111 << 12) |      /* funct3 */
               (imm_4_1 << 8) |     /* imm[4:1] */
               (imm_11 << 7) |      /* imm[11] */
               0b1100011;           /* opcode */
    }
}