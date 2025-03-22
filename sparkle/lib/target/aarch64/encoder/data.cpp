#include <cstdint>
#include <sparkle/target/aarch64/common.hpp>
#include <sparkle/target/aarch64/encoding/data.hpp>

namespace sprk::aarch64
{
    /* MOV (register) is actually an alias of ORR with XZR as the first source */
    uint32_t encode_mov_reg(const int rd, const int rm, const bool is64bit)
    {
        const uint32_t sf = is64bit ? 1 : 0;        /* 64-bit (1) or 32-bit (0) */
        constexpr  uint32_t opc = 1;                     /* ORR=1 */
        constexpr uint32_t N = 0;                       /* no inversion */
        constexpr uint32_t shift_type = 0;              /* LSL */
        constexpr uint32_t shift_amount = 0;            /* no shift */
        constexpr uint32_t rn = 31;                     /* XZR/WZR (register 31) */
        
        return (sf << 31) |         /* size flag */
               (opc << 29) |        /* operation: 1 for ORR */
               (0b01010 << 24) |    /* opcode fixed pattern */
               (shift_type << 22) | /* shift type (LSL) */
               (N << 21) |          /* N bit (0) */
               (rm << 16) |         /* source register */
               (shift_amount << 10) | /* shift amount (0) */
               (rn << 5) |          /* XZR/WZR as first source */
               rd;                  /* destination register */
    }

    uint32_t encode_mvn_reg(const int rd, int rm, const uint32_t shift_type, const uint32_t shift_amount, const bool is64bit)
    {
        const uint32_t sf = is64bit ? 1 : 0;        /* 64-bit (1) or 32-bit (0) */
        constexpr uint32_t opc = 1;                     /* ORN=1+N */
        constexpr uint32_t N = 1;                       /* invert rm */
        constexpr uint32_t rn = 31;                     /* XZR/WZR (register 31) */
        
        /* shift type: 00=LSL, 01=LSR, 10=ASR, 11=ROR */
        if (is64bit == false && shift_amount >= 32)
            /* for 32-bit registers, shift amount must be < 32 */
            return 0; /* Error condition */
        
        return (sf << 31) |         /* size flag */
               (opc << 29) |        /* operation: 1 for ORN */
               (0b01010 << 24) |    /* opcode fixed pattern */
               (shift_type << 22) | /* shift type */
               (N << 21) |          /* N bit (1 for inversion) */
               (rm << 16) |         /* source register */
               ((shift_amount & 0x3F) << 10) | /* shift amount */
               (rn << 5) |          /* XZR/WZR as first source */
               rd;                  /* destination register */
    }

    uint32_t encode_mov_imm(const int rd, const uint64_t imm, const bool is64bit)
    {
        /* if the immediate fits in 16 bits, use MOVZ */
        if (imm <= 0xFFFF)
            return encode_movz(rd, imm, 0, is64bit);

        /* if the immediate with all bits inverted fits in 16 bits, use MOVN */
        if ((~imm & (is64bit ? 0xFFFFFFFFFFFFFFFF : 0xFFFFFFFF)) <= 0xFFFF)
            return encode_movn(rd, ~imm & 0xFFFF, 0, is64bit);

        /* for other cases, try to encode as a logical immediate with ORR */
        uint32_t N, immr, imms;
        if (encode_logical_imm(imm, is64bit, &N, &immr, &imms)) 
        {
            const uint32_t sf = is64bit ? 1 : 0;    /* 64-bit (1) or 32-bit (0) */
            constexpr uint32_t opc = 1;                 /* ORR=1 */
            constexpr uint32_t rn = 31;                 /* XZR/WZR (register 31) */
            
            return (sf << 31) |      /* size flag */
                (opc << 29) |        /* operation: 1 for ORR */
                (0b100100 << 23) |   /* opcode fixed pattern */
                (N << 22) |          /* N bit from logical immediate */
                (immr << 16) |       /* immr from logical immediate */
                (imms << 10) |       /* imms from logical immediate */
                (rn << 5) |          /* XZR/WZR as source register */
                rd;                  /* destination register */
        }

        /* if the immediate doesn't fit any of these patterns, it needs to be broken down */
        /* into multiple instructions (MOVZ+MOVK or similar), which is beyond the scope */
        /* of this encoder. Return 0 to indicate failure. */
        return 0;
    }

    uint32_t encode_movz(const int rd, const uint16_t imm16, const uint32_t shift, const bool is64bit)
    {
        const uint32_t sf = is64bit ? 1 : 0;    /* 64-bit (1) or 32-bit (0) */
        constexpr uint32_t opc = 2;                 /* MOVZ=2 */
        
        /* conv shift (0, 16, 32, 48) to hw field (0, 1, 2, 3) */
        const uint32_t hw = shift / 16;
        
        /* 32-bit registers can only use shifts 0 and 16 (hw=0,1) */
        if (!is64bit && hw > 1)
            return 0; /* error */
        
        return (sf << 31) |         /* size flag */
               (opc << 29) |        /* operation: 2 for MOVZ */
               (0b100101 << 23) |   /* opcode fixed pattern */
               (hw << 21) |         /* hw field (shift/16) */
               ((imm16 & 0xFFFF) << 5) | /* 16-bit immediate */
               rd;                  /* destination register */
    }

    uint32_t encode_movn(int rd, uint16_t imm16, uint32_t shift, bool is64bit)
    {
        const uint32_t sf = is64bit ? 1 : 0;    /* 64-bit (1) or 32-bit (0) */
        constexpr uint32_t opc = 0;                 /* MOVN=0 */
        
        /* conv shift (0, 16, 32, 48) to hw field (0, 1, 2, 3) */
        const uint32_t hw = shift / 16;
        
        /* 32-bit registers can only use shifts 0 and 16 (hw=0,1) */
        if (!is64bit && hw > 1)
            return 0; /* Error */
        
        return (sf << 31) |         /* size flag */
               (opc << 29) |        /* operation: 0 for MOVN */
               (0b100101 << 23) |   /* opcode fixed pattern */
               (hw << 21) |         /* hw field (shift/16) */
               ((imm16 & 0xFFFF) << 5) | /* 16-bit immediate */
               rd;                  /* destination register */
    }

    uint32_t encode_movk(int rd, uint16_t imm16, uint32_t shift, bool is64bit)
    {
        const uint32_t sf = is64bit ? 1 : 0;    /* 64-bit (1) or 32-bit (0) */
        constexpr uint32_t opc = 3;                 /* MOVK=3 */
        
        /* conv shift (0, 16, 32, 48) to hw field (0, 1, 2, 3) */
        const uint32_t hw = shift / 16;
        
        /* 32-bit registers can only use shifts 0 and 16 (hw=0,1) */
        if (!is64bit && hw > 1)
            return 0; /* error */
        
        return (sf << 31) |         /* size flag */
               (opc << 29) |        /* operation: 3 for MOVK */
               (0b100101 << 23) |   /* opcode fixed pattern */
               (hw << 21) |         /* hw field (shift/16) */
               ((imm16 & 0xFFFF) << 5) | /* 16-bit immediate */
               rd;                  /* destination register */
    }
}