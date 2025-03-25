#pragma once

#include <sparkle/target/risc-v/encoding/fp.hpp>

namespace sprk::riscv 
{
    uint32_t encode_flw(int rd, int rs1, int offset)
    {
        /* imm is 12 bits, sign-extended */
        int imm12 = offset & 0xFFF;
        
        return (imm12 << 20) |      /* imm[11:0] */
               (rs1 << 15) |        /* rs1 */
               (0b010 << 12) |      /* funct3 = W */
               (rd << 7) |          /* rd */
               0b0000111;           /* opcode */
    }

    uint32_t encode_fsw(int rs2, int rs1, int offset)
    {
        /* imm is 12 bits, sign-extended */
        int imm12 = offset & 0xFFF;
        
        return ((imm12 >> 5) << 25) | /* imm[11:5] */
               (rs2 << 20) |          /* rs2 */
               (rs1 << 15) |          /* rs1 */
               (0b010 << 12) |        /* funct3 = W */
               ((imm12 & 0x1F) << 7) | /* imm[4:0] */
               0b0100111;             /* opcode */
    }

    uint32_t encode_fld(int rd, int rs1, int offset)
    {
        /* imm is 12 bits, sign-extended */
        int imm12 = offset & 0xFFF;
        
        return (imm12 << 20) |      /* imm[11:0] */
               (rs1 << 15) |        /* rs1 */
               (0b011 << 12) |      /* funct3 = D */
               (rd << 7) |          /* rd */
               0b0000111;           /* opcode */
    }

    uint32_t encode_fsd(int rs2, int rs1, int offset)
    {
        /* imm is 12 bits, sign-extended */
        int imm12 = offset & 0xFFF;
        
        return ((imm12 >> 5) << 25) | /* imm[11:5] */
               (rs2 << 20) |          /* rs2 */
               (rs1 << 15) |          /* rs1 */
               (0b011 << 12) |        /* funct3 = D */
               ((imm12 & 0x1F) << 7) | /* imm[4:0] */
               0b0100111;             /* opcode */
    }

    uint32_t encode_fadd_s(int rd, int rs1, int rs2, int rm)
    {
        return (0b0000000 << 25) |  /* funct7 */
               (rs2 << 20) |        /* rs2 */
               (rs1 << 15) |        /* rs1 */
               (rm << 12) |         /* rounding mode */
               (rd << 7) |          /* rd */
               0b1010011;           /* opcode */
    }

    uint32_t encode_fsub_s(int rd, int rs1, int rs2, int rm)
    {
        return (0b0000100 << 25) |  /* funct7 */
               (rs2 << 20) |        /* rs2 */
               (rs1 << 15) |        /* rs1 */
               (rm << 12) |         /* rounding mode */
               (rd << 7) |          /* rd */
               0b1010011;           /* opcode */
    }

    uint32_t encode_fmul_s(int rd, int rs1, int rs2, int rm)
    {
        return (0b0001000 << 25) |  /* funct7 */
               (rs2 << 20) |        /* rs2 */
               (rs1 << 15) |        /* rs1 */
               (rm << 12) |         /* rounding mode */
               (rd << 7) |          /* rd */
               0b1010011;           /* opcode */
    }

    uint32_t encode_fdiv_s(int rd, int rs1, int rs2, int rm)
    {
        return (0b0001100 << 25) |  /* funct7 */
               (rs2 << 20) |        /* rs2 */
               (rs1 << 15) |        /* rs1 */
               (rm << 12) |         /* rounding mode */
               (rd << 7) |          /* rd */
               0b1010011;           /* opcode */
    }

    uint32_t encode_fsqrt_s(int rd, int rs1, int rm)
    {
        return (0b0101100 << 25) |  /* funct7 */
               (0b00000 << 20) |    /* rs2 = 0 */
               (rs1 << 15) |        /* rs1 */
               (rm << 12) |         /* rounding mode */
               (rd << 7) |          /* rd */
               0b1010011;           /* opcode */
    }

    uint32_t encode_fmadd_s(int rd, int rs1, int rs2, int rs3, int rm)
    {
        return (rs3 << 27) |        /* rs3 */
               (0b00 << 25) |       /* fmt = S */
               (rs2 << 20) |        /* rs2 */
               (rs1 << 15) |        /* rs1 */
               (rm << 12) |         /* rounding mode */
               (rd << 7) |          /* rd */
               0b1000011;           /* opcode */
    }

    uint32_t encode_fmsub_s(int rd, int rs1, int rs2, int rs3, int rm)
    {
        return (rs3 << 27) |        /* rs3 */
               (0b00 << 25) |       /* fmt = S */
               (rs2 << 20) |        /* rs2 */
               (rs1 << 15) |        /* rs1 */
               (rm << 12) |         /* rounding mode */
               (rd << 7) |          /* rd */
               0b1000111;           /* opcode */
    }

    uint32_t encode_feq_s(int rd, int rs1, int rs2)
    {
        return (0b1010000 << 25) |  /* funct7 */
            (rs2 << 20) |        /* rs2 */
            (rs1 << 15) |        /* rs1 */
            (0b010 << 12) |      /* funct3 = Equal */
            (rd << 7) |          /* rd */
            0b1010011;           /* opcode */
    }

    uint32_t encode_flt_s(int rd, int rs1, int rs2)
    {
        return (0b1010000 << 25) |  /* funct7 */
            (rs2 << 20) |        /* rs2 */
            (rs1 << 15) |        /* rs1 */
            (0b001 << 12) |      /* funct3 = Less Than */
            (rd << 7) |          /* rd */
            0b1010011;           /* opcode */
    }

    uint32_t encode_fle_s(int rd, int rs1, int rs2)
    {
        return (0b1010000 << 25) |  /* funct7 */
            (rs2 << 20) |        /* rs2 */
            (rs1 << 15) |        /* rs1 */
            (0b000 << 12) |      /* funct3 = Less Than or Equal */
            (rd << 7) |          /* rd */
            0b1010011;           /* opcode */
    }

    uint32_t encode_fsgnj_s(int rd, int rs1, int rs2)
    {
        return (0b0010000 << 25) |  /* funct7 */
            (rs2 << 20) |        /* rs2 */
            (rs1 << 15) |        /* rs1 */
            (0b000 << 12) |      /* funct3 = Sign Injection */
            (rd << 7) |          /* rd */
            0b1010011;           /* opcode */
    }

    uint32_t encode_fsgnjn_s(int rd, int rs1, int rs2)
    {
        return (0b0010000 << 25) |  /* funct7 */
            (rs2 << 20) |        /* rs2 */
            (rs1 << 15) |        /* rs1 */
            (0b001 << 12) |      /* funct3 = Negated Sign Injection */
            (rd << 7) |          /* rd */
            0b1010011;           /* opcode */
    }

    uint32_t encode_fsgnjx_s(int rd, int rs1, int rs2)
    {
        return (0b0010000 << 25) |  /* funct7 */
            (rs2 << 20) |        /* rs2 */
            (rs1 << 15) |        /* rs1 */
            (0b010 << 12) |      /* funct3 = XOR Sign Injection */
            (rd << 7) |          /* rd */
            0b1010011;           /* opcode */
    }

    uint32_t encode_fmin_s(int rd, int rs1, int rs2)
    {
        return (0b0010100 << 25) |  /* funct7 */
            (rs2 << 20) |        /* rs2 */
            (rs1 << 15) |        /* rs1 */
            (0b000 << 12) |      /* funct3 = Minimum */
            (rd << 7) |          /* rd */
            0b1010011;           /* opcode */
    }

    uint32_t encode_fmax_s(int rd, int rs1, int rs2)
    {
        return (0b0010100 << 25) |  /* funct7 */
            (rs2 << 20) |        /* rs2 */
            (rs1 << 15) |        /* rs1 */
            (0b001 << 12) |      /* funct3 = Maximum */
            (rd << 7) |          /* rd */
            0b1010011;           /* opcode */
    }

    uint32_t encode_fclass_s(int rd, int rs1)
    {
        return (0b1110000 << 25) |  /* funct7 */
            (0b00000 << 20) |    /* rs2 = 0 */
            (rs1 << 15) |        /* rs1 */
            (0b001 << 12) |      /* funct3 = Classification */
            (rd << 7) |          /* rd */
            0b1010011;           /* opcode */
    }

    uint32_t encode_fcvt_w_s(int rd, int rs1, int rm)
    {
        return (0b1100000 << 25) |  /* funct7 */
            (0b00000 << 20) |    /* rs2 = 0 (W conversion) */
            (rs1 << 15) |        /* rs1 */
            (rm << 12) |         /* rounding mode */
            (rd << 7) |          /* rd */
            0b1010011;           /* opcode */
    }

    uint32_t encode_fcvt_wu_s(int rd, int rs1, int rm)
    {
        return (0b1100000 << 25) |  /* funct7 */
            (0b00001 << 20) |    /* rs2 = 1 (unsigned W conversion) */
            (rs1 << 15) |        /* rs1 */
            (rm << 12) |         /* rounding mode */
            (rd << 7) |          /* rd */
            0b1010011;           /* opcode */
    }

    uint32_t encode_fcvt_s_w(int rd, int rs1, int rm)
    {
        return (0b1101000 << 25) |  /* funct7 */
            (0b00000 << 20) |    /* rs2 = 0 (W conversion) */
            (rs1 << 15) |        /* rs1 */
            (rm << 12) |         /* rounding mode */
            (rd << 7) |          /* rd */
            0b1010011;           /* opcode */
    }

    uint32_t encode_fcvt_s_wu(int rd, int rs1, int rm)
    {
        return (0b1101000 << 25) |  /* funct7 */
            (0b00001 << 20) |    /* rs2 = 1 (unsigned W conversion) */
            (rs1 << 15) |        /* rs1 */
            (rm << 12) |         /* rounding mode */
            (rd << 7) |          /* rd */
            0b1010011;           /* opcode */
    }

    uint32_t encode_fnmsub_s(int rd, int rs1, int rs2, int rs3, int rm)
    {
        return (rs3 << 27) |        /* rs3 */
            (0b00 << 25) |       /* fmt = S */
            (rs2 << 20) |        /* rs2 */
            (rs1 << 15) |        /* rs1 */
            (rm << 12) |         /* rounding mode */
            (rd << 7) |          /* rd */
            0b1001011;           /* opcode */
    }

    uint32_t encode_fnmadd_s(int rd, int rs1, int rs2, int rs3, int rm)
    {
        return (rs3 << 27) |        /* rs3 */
            (0b00 << 25) |       /* fmt = S */
            (rs2 << 20) |        /* rs2 */
            (rs1 << 15) |        /* rs1 */
            (rm << 12) |         /* rounding mode */
            (rd << 7) |          /* rd */
            0b1001111;           /* opcode */
    }

    uint32_t encode_fcvt_l_s(int rd, int rs1, int rm)
    {
        return (0b1100000 << 25) |  /* funct7 */
            (0b00010 << 20) |    /* rs2 = 2 (64-bit signed conversion) */
            (rs1 << 15) |        /* rs1 */
            (rm << 12) |         /* rounding mode */
            (rd << 7) |          /* rd */
            0b1010011;           /* opcode */
    }

    uint32_t encode_fcvt_lu_s(int rd, int rs1, int rm)
    {
        return (0b1100000 << 25) |  /* funct7 */
            (0b00011 << 20) |    /* rs2 = 3 (64-bit unsigned conversion) */
            (rs1 << 15) |        /* rs1 */
            (rm << 12) |         /* rounding mode */
            (rd << 7) |          /* rd */
            0b1010011;           /* opcode */
    }

    uint32_t encode_fcvt_s_l(int rd, int rs1, int rm)
    {
        return (0b1101000 << 25) |  /* funct7 */
            (0b00010 << 20) |    /* rs2 = 2 (64-bit signed conversion) */
            (rs1 << 15) |        /* rs1 */
            (rm << 12) |         /* rounding mode */
            (rd << 7) |          /* rd */
            0b1010011;           /* opcode */
    }

    uint32_t encode_fcvt_s_lu(int rd, int rs1, int rm)
    {
        return (0b1101000 << 25) |  /* funct7 */
            (0b00011 << 20) |    /* rs2 = 3 (64-bit unsigned conversion) */
            (rs1 << 15) |        /* rs1 */
            (rm << 12) |         /* rounding mode */
            (rd << 7) |          /* rd */
            0b1010011;           /* opcode */
    }

    uint32_t encode_fmv_x_s(int rd, int rs1)
    {
        return (0b1110000 << 25) |  /* funct7 */
            (0b00000 << 20) |    /* rs2 = 0 */
            (rs1 << 15) |        /* rs1 */
            (0b000 << 12) |      /* funct3 */
            (rd << 7) |          /* rd */
            0b1010011;           /* opcode */
    }

    uint32_t encode_fmv_s_x(int rd, int rs1)
    {
        return (0b1111000 << 25) |  /* funct7 */
            (0b00000 << 20) |    /* rs2 = 0 */
            (rs1 << 15) |        /* rs1 */
            (0b000 << 12) |      /* funct3 */
            (rd << 7) |          /* rd */
            0b1010011;           /* opcode */
    }
}