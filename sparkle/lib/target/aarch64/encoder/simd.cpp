#include <sparkle/target/aarch64/encoding/simd.hpp>
#include <sparkle/target/aarch64/common.hpp>

namespace sprk::aarch64
{
    uint32_t encode_movi(int rd, uint64_t imm, uint32_t arrangement)
    {
        uint32_t Q = 0;     /* Q=0 for 8B/4H/2S, Q=1 for 16B/8H/4S/2D */
        uint32_t op = 0;    /* op=0 for MOVI, op=1 for MVNI */
        uint32_t cmode, imm_bits;
        
        /* determine Q bit based on arrangement */
        switch (arrangement) 
        {
            case SIMD_8B: /* 8B (8 bytes) */
                Q = 0;
                break;
            case SIMD_16B: /* 16B (16 bytes) */
                Q = 1;
                break;
            case SIMD_4H: /* 4H (4 halfwords) */
                Q = 0;
                break;
            case SIMD_8H: /* 8H (8 halfwords) */
                Q = 1;
                break;
            case SIMD_2S: /* 2S (2 words) */
                Q = 0;
                break;
            case SIMD_4S: /* 4S (4 words) */
                Q = 1;
                break;
            case SIMD_2D: /* 2D (2 doublewords) - only available for certain immediates; may need to handle more cases if this breaks */
                Q = 1;
                break;
            default:
                return 0; /* error: invalid arrangement */
        }
        
        /* Determine cmode and encode the immediate */
        if (arrangement <= SIMD_16B) 
        {
            /* 8B/16B: 8-bit immediate */
            cmode = 0xE;
            imm_bits = imm & 0xFF;
        } 
        else if (arrangement <= SIMD_8H) 
        {
            /* 4H/8H: 16-bit immediate (various patterns) */
            if ((imm & 0xFF) == imm) 
            {
                cmode = 0x8; /* 8-bit immediate, zero extended */
                imm_bits = imm & 0xFF;
            } 
            else if ((imm & 0xFF00) == imm) 
            {
                cmode = 0xA; /* 8-bit immediate, shifted to upper byte */
                imm_bits = (imm >> 8) & 0xFF;
            } 
            else 
            {
                return 0; /* Not a supported immediate pattern */
            }
        } 
        else if (arrangement <= SIMD_4S) 
        {
            /* 2S/4S: 32-bit immediate (various patterns) */
            if ((imm & 0xFF) == imm) 
            {
                cmode = 0x0; /* 8-bit immediate, zero extended */
                imm_bits = imm & 0xFF;
            } 
            else if ((imm & 0xFF00) == imm) 
            {
                cmode = 0x2; /* 8-bit immediate, shifted to byte 1 */
                imm_bits = (imm >> 8) & 0xFF;
            } 
            else if ((imm & 0xFF0000) == imm) 
            {
                cmode = 0x4; /* 8-bit immediate, shifted to byte 2 */
                imm_bits = (imm >> 16) & 0xFF;
            } 
            else if ((imm & 0xFF000000) == imm) 
            {
                cmode = 0x6;/* 8-bit immediate, shifted to byte 3 */
                imm_bits = (imm >> 24) & 0xFF;
            } 
            else 
            {
                return 0; /* not a supported immediate pattern */
            }
        } 
        else if (arrangement == SIMD_2D) 
        {
            /* 2D: only special bit patterns are supported */
            return 0;  /* complex case, needs a separate encoder */
        }
        
        /* encode the actual instruction */
        return (Q << 30) |                /* Q bit for arrangement */
               (op << 29) |               /* Operation: 0 for MOVI */
               (0b0111100000 << 19) |     /* opcode fixed pattern */
               (((imm_bits >> 5) & 0x7) << 16) | /* bits 7:5 of immediate to immh */
               (cmode << 12) |            /* cmode field */
               (0 << 11) |                /* o2=0 for integer variant */
               (1 << 10) |                /* fixed bit */
               ((imm_bits & 0x1F) << 5) | /* bits 4:0 of immediate to imml */
               rd;                        /* destination vector register */
    }

    uint32_t encode_add_simd(int rd, int rn, int rm, uint32_t arrangement)
    {
        uint32_t Q = 0;     /* Q=0 for 8B/4H/2S, Q=1 for 16B/8H/4S/2D */
        uint32_t U = 0;     /* U=0 for ADD */
        uint32_t size;      /* 00=8-bit, 01=16-bit, 10=32-bit, 11=64-bit */
        
        /* determine Q bit and size based on arrangement */
        switch (arrangement) 
        {
            case SIMD_8B: /* 8B (8 bytes) */
                Q = 0;
                size = 0;
                break;
            case SIMD_16B: /* 16B (16 bytes) */
                Q = 1;
                size = 0;
                break;
            case SIMD_4H: /* 4H (4 halfwords) */
                Q = 0;
                size = 1;
                break;
            case SIMD_8H: /* 8H (8 halfwords) */
                Q = 1;
                size = 1;
                break;
            case SIMD_2S: /* 2S (2 words) */
                Q = 0;
                size = 2;
                break;
            case SIMD_4S: /* 4S (4 words) */
                Q = 1;
                size = 2;
                break;
            case SIMD_2D: /* 2D (2 doublewords) */
                Q = 1;
                size = 3;
                break;
            default:
                return 0; /* error: invalid arrangement */
        }
        
        return (Q << 30) |                /* Q bit for arrangement */
               (U << 29) |                /* U=0 for ADD */
               (0b01110 << 24) |          /* opcode fixed pattern */
               (size << 22) |             /* size field */
               (1 << 21) |                /* fixed bit */
               (rm << 16) |               /* second source register */
               (0b10000 << 11) |          /* fixed pattern for ADD */
               (1 << 10) |                /* fixed bit */
               (rn << 5) |                /* first source register */
               rd;                        /* destination register */
    }

    uint32_t encode_sub_simd(int rd, int rn, int rm, uint32_t arrangement)
    {
        uint32_t Q = 0;     /* Q=0 for 8B/4H/2S, Q=1 for 16B/8H/4S/2D */
        uint32_t U = 1;     /* U=1 for SUB */
        uint32_t size;      /* 00=8-bit, 01=16-bit, 10=32-bit, 11=64-bit */
        
        /* determine Q bit and size based on arrangement */
        switch (arrangement) 
        {
            case SIMD_8B: /* 8B (8 bytes) */
                Q = 0;
                size = 0;
                break;
            case SIMD_16B: /* 16B (16 bytes) */
                Q = 1;
                size = 0;
                break;
            case SIMD_4H: /* 4H (4 halfwords) */
                Q = 0;
                size = 1;
                break;
            case SIMD_8H: /* 8H (8 halfwords) */
                Q = 1;
                size = 1;
                break;
            case SIMD_2S: /* 2S (2 words) */
                Q = 0;
                size = 2;
                break;
            case SIMD_4S: /* 4S (4 words) */
                Q = 1;
                size = 2;
                break;
            case SIMD_2D: /* 2D (2 doublewords) */
                Q = 1;
                size = 3;
                break;
            default:
                return 0; /* error: invalid arrangement */
        }
        
        return (Q << 30) |                /* Q bit for arrangement */
            (U << 29) |                /* U=1 for SUB */
            (0b01110 << 24) |          /* opcode fixed pattern */
            (size << 22) |             /* size field */
            (1 << 21) |                /* fixed bit */
            (rm << 16) |               /* second source register */
            (0b10000 << 11) |          /* fixed pattern for SUB */
            (1 << 10) |                /* fixed bit */
            (rn << 5) |                /* first source register */
            rd;                        /* destination register */
    }

    uint32_t encode_mul_simd(int rd, int rn, int rm, uint32_t arrangement)
    {
        uint32_t Q = 0;     /* Q=0 for 8B/4H/2S, Q=1 for 16B/8H/4S */
        uint32_t U = 0;     /* U=0 for MUL */
        uint32_t size;      /* 00=8-bit, 01=16-bit, 10=32-bit */
        
        /* determine Q bit and size based on arrangement */
        switch (arrangement)
        {
            case SIMD_8B: /* 8B (8 bytes) */
                Q = 0;
                size = 0;
                break;
            case SIMD_16B: /* 16B (16 bytes) */
                Q = 1;
                size = 0;
                break;
            case SIMD_4H: /* 4H (4 halfwords) */
                Q = 0;
                size = 1;
                break;
            case SIMD_8H: /* 8H (8 halfwords) */
                Q = 1;
                size = 1;
                break;
            case SIMD_2S: /* 2S (2 words) */
                Q = 0;
                size = 2;
                break;
            case SIMD_4S: /* 4S (4 words) */
                Q = 1;
                size = 2;
                break;
            default:
                return 0; /* error: invalid arrangement for MUL (64-bit not supported) */
        }
        
        return (Q << 30) |                /* Q bit for arrangement */
               (U << 29) |                /* U=0 for MUL */
               (0b01110 << 24) |          /* opcode fixed pattern */
               (size << 22) |             /* Size field */
               (1 << 21) |                /* fixed bit */
               (rm << 16) |               /* third register */
               (1 << 15) |                /* fixed bit */
               (0b001 << 12) |            /* fixed pattern */
               (1 << 11) |                /* fixed bit for MUL */
               (1 << 10) |                /* fixed bit */
               (rn << 5) |                /* second register */
               rd;                        /* destination register */
    }

    uint32_t encode_and_simd(int rd, int rn, int rm, uint32_t arrangement)
    {
        uint32_t Q = 0;     /* Q=0 for 8B, Q=1 for 16B */
        uint32_t U = 0;     /* U=0 for AND */
        uint32_t size = 0;  /* always 0 for logical operations */
        
        /* determine Q bit based on arrangement */
        switch (arrangement) 
        {
            case 0: /* 8B (8 bytes) */
                Q = 0;
                break;
            case 1: /* 16B (16 bytes) */
                Q = 1;
                break;
            default:
                return 0; /* error: invalid arrangement for AND */
        }
        
        return (Q << 30) |                /* Q bit for arrangement */
               (U << 29) |                /* U=0 for AND */
               (0b01110 << 24) |          /* opcode fixed pattern */
               (size << 22) |             /* Size field (0 for logical) */
               (1 << 21) |                /* fixed bit */
               (rm << 16) |               /* second source register */
               (0b00011 << 11) |          /* fixed pattern for AND */
               (1 << 10) |                /* fixed bit */
               (rn << 5) |                /* first source register */
               rd;                        /* destination register */
    }

    uint32_t encode_orr_simd(int rd, int rn, int rm, uint32_t arrangement)
    {
        uint32_t Q = 0;     /* Q=0 for 8B, Q=1 for 16B */
        uint32_t U = 0;     /* U=0 for first logical group */
        uint32_t size = 2;  /* always 2 for ORR */
        
        /* determine Q bit based on arrangement */
        switch (arrangement) 
        {
            case SIMD_8B: /* 8B (8 bytes) */
                Q = 0;
                break;
            case SIMD_16B: /* 16B (16 bytes) */
                Q = 1;
                break;
            default:
                return 0; /* error: invalid arrangement for ORR */
        }
        
        return (Q << 30) |                /* Q bit for arrangement */
               (U << 29) |                /* U=0 for first logical group */
               (0b01110 << 24) |          /* opcode fixed pattern */
               (size << 22) |             /* Size field (2 for ORR) */
               (1 << 21) |                /* fixed bit */
               (rm << 16) |               /* second source register */
               (0b00011 << 11) |          /* fixed pattern for logical */
               (1 << 10) |                /* fixed bit */
               (rn << 5) |                /* first source register */
               rd;                        /* destination register */
    }

    uint32_t encode_tbl(int rd, int rn, int rm, uint32_t len, bool is16b)
    {
        /* len: 0=1 register, 1=2 registers, 2=3 registers, 3=4 registers */
        uint32_t Q = is16b ? 1 : 0;    /* Q=1 for 16B, Q=0 for 8B */
        uint32_t op = 0;               /* op=0 for TBL (no fault on out-of-range) */
        uint32_t size = 0;             /* size=0 for byte operations */
        
        if (len > 3)
            return 0; /* error: invalid length */
        
        return (Q << 30) |             /* Q bit for vector length */
            (0b001110 << 24) |      /* opcode fixed pattern */
            (size << 22) |          /* size field (0 for byte) */
            (0 << 21) |             /* fixed bit */
            (rm << 16) |            /* index vector register */
            (0 << 15) |             /* fixed bit */
            (len << 13) |           /* length field (0-3) */
            (op << 12) |            /* op bit (0 for TBL) */
            (0 << 11) |             /* fixed bit */
            (0 << 10) |             /* fixed bit */
            (rn << 5) |             /* table base register */
            rd;                     /* destination register */
    }

    uint32_t encode_dup_elem(int rd, int rn, uint32_t index, uint32_t arrangement)
    {
        uint32_t Q = 0;     /* Q=0 for 8B/4H/2S, Q=1 for 16B/8H/4S/2D */
        uint32_t imm5 = 0;  /* encoded index and size */
        uint32_t op = 0;    /* op=0 for DUP from element */
        
        /* determine Q bit and encode imm5 based on arrangement */
        uint32_t size = 0;
        switch (arrangement) 
        {
            case 0: /* 8B (8 bytes) */
                Q = 0;
                size = 0;
                if (index > 15) return 0; /* error: index out of range */
                imm5 = (index << 1) | 1;  /* encode for byte */
                break;
            case 1: /* 16B (16 bytes) */
                Q = 1;
                size = 0;
                if (index > 15) return 0; /* error: index out of range */
                imm5 = (index << 1) | 1;  /* encode for byte */
                break;
            case 2: /* 4H (4 halfwords) */
                Q = 0;
                size = 1;
                if (index > 7) return 0;  /* error: index out of range */
                imm5 = (index << 2) | 2;  /* encode for halfword */
                break;
            case 3: /* 8H (8 halfwords) */
                Q = 1;
                size = 1;
                if (index > 7) return 0;  /* error: index out of range */
                imm5 = (index << 2) | 2;  /* encode for halfword */
                break;
            case 4: /* 2S (2 words) */
                Q = 0;
                size = 2;
                if (index > 3) return 0;  /* error: index out of range */
                imm5 = (index << 3) | 4;  /* encode for word */
                break;
            case 5: /* 4S (4 words) */
                Q = 1;
                size = 2;
                if (index > 3) return 0;  /* error: index out of range */
                imm5 = (index << 3) | 4;  /* encode for word */
                break;
            case 6: /* 2D (2 doublewords) */
                Q = 1;
                size = 3;
                if (index > 1) return 0;  /* error: index out of range */
                imm5 = (index << 4) | 8;  /* encode for doubleword */
                break;
            default:
                return 0; /* error: invalid arrangement */
    }
    
    return (Q << 30) |             /* Q bit for vector length */
           (0b001110 << 24) |      /* opcode fixed pattern */
           (0 << 22) |             /* fixed bits */
           (0 << 21) |             /* fixed bit */
           (imm5 << 16) |          /* encoded index and size */
           (0 << 15) |             /* fixed bit */
           (0 << 14) |             /* fixed bit */
           (0 << 13) |             /* fixed bit */
           (0 << 12) |             /* fixed bit */
           (op << 11) |            /* op bit (0 for element) */
           (1 << 10) |             /* fixed bit */
           (rn << 5) |             /* source register */
           rd;                     /* destination register */
    }

    uint32_t encode_dup_gen(int rd, int rn, uint32_t arrangement)
    {
        uint32_t Q = 0;     /* Q=0 for 8B/4H/2S, Q=1 for 16B/8H/4S/2D */
        uint32_t imm5 = 0;  /* dncoded size */
        uint32_t op = 1;    /* op=1 for DUP from general register */
        
        /* determine Q bit and encode imm5 based on arrangement */
        switch (arrangement) 
        {
            case SIMD_8B: /* 8B (8 bytes) */
                Q = 0;
                imm5 = 1;  /* size for byte */
                break;
            case SIMD_16B: /* 16B (16 bytes) */
                Q = 1;
                imm5 = 1;  /* size for byte */
                break;
            case SIMD_4H: /* 4H (4 halfwords) */
                Q = 0;
                imm5 = 2;  /* size for halfword */
                break;
            case SIMD_8H: /* 8H (8 halfwords) */
                Q = 1;
                imm5 = 2;  /* size for halfword */
                break;
            case SIMD_2S: /* 2S (2 words) */
                Q = 0;
                imm5 = 4;  /* size for word */
                break;
            case SIMD_4S: /* 4S (4 words) */
                Q = 1;
                imm5 = 4;  /* size for word */
                break;
            case SIMD_2D: /* 2D (2 doublewords) */
                Q = 1;
                imm5 = 8;  /* size for doubleword */
                break;
            default:
                return 0; /* error: invalid arrangement */
        }
        
        return (Q << 30) |             /* Q bit for vector length */
               (0b001110 << 24) |      /* opcode fixed pattern */
               (0 << 22) |             /* fixed bits */
               (0 << 21) |             /* fixed bit */
               (imm5 << 16) |          /* encoded size */
               (0 << 15) |             /* fixed bit */
               (0 << 14) |             /* fixed bit */
               (0 << 13) |             /* fixed bit */
               (0 << 12) |             /* fixed bit */
               (op << 11) |            /* op bit (1 for general register) */
               (1 << 10) |             /* fixed bit */
               (rn << 5) |             /* source register */
               rd;                     /* destination register */
    }
}