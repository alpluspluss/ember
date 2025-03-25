#include <sparkle/target/risc-v/encoding/arith.hpp>
#include <sparkle/target/risc-v/encoding/data.hpp>

namespace sprk::riscv
{
    uint32_t encode_jal(int rd, int offset)
    {
        /* imm must be a PO2 */
        if (offset & 0x1) 
        {
            offset &= ~1;
        }
        if (offset < -1048576 || offset > 1048574) 
        {
            /* if out of range, set the offset to maximum */
            offset = (offset < 0) ? -1048576 : 1048574;
        }
        
        /* extract bits to get the J-type immediate */
        int imm20 = (offset >> 1) & 0xFFFFF;
        int imm_20 = (imm20 >> 19) & 0x1;
        int imm_19_12 = (imm20 >> 11) & 0xFF;
        int imm_11 = (imm20 >> 10) & 0x1;
        int imm_10_1 = imm20 & 0x3FF;
        
        return (imm_20 << 31) |     /* imm[20] */
               (imm_10_1 << 21) |   /* imm[10:1] */
               (imm_11 << 20) |     /* imm[11] */
               (imm_19_12 << 12) |  /* imm[19:12] */
               (rd << 7) |          /* rd */
               0b1101111;           /* opcode */
    }

    uint32_t encode_jalr(int rd, int rs1, int offset)
    {
        /* imm is 12 bits, sign-extended */
        int imm12 = offset & 0xFFF;
        
        return (imm12 << 20) |      /* imm[11:0] */
               (rs1 << 15) |        /* rs1 */
               (0b000 << 12) |      /* funct3 */
               (rd << 7) |          /* rd */
               0b1100111;           /* opcode */
    }

    uint32_t encode_lui(int rd, int imm)
    {
        /* get the top 20 bits of immediate */
        int imm20 = (imm >> 12) & 0xFFFFF;
        
        return (imm20 << 12) |      /* imm[31:12] */
               (rd << 7) |          /* rd */
               0b0110111;           /* opcode */
    }

    uint32_t encode_auipc(int rd, int imm)
    {
        /* get the top 20 bits of immediate */
        int imm20 = (imm >> 12) & 0xFFFFF;
        
        return (imm20 << 12) |      /* imm[31:12] */
               (rd << 7) |          /* rd */
               0b0010111;           /* opcode */
    }

    /* NOTE: MV is a pseudo-instruction that uses ADDI */
    uint32_t encode_mv(int rd, int rs1)
    {
        return encode_addi(rd, rs1, 0);
    }
}