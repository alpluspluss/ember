#include <sparkle/target/aarch64/encoding/arith.hpp>

namespace sprk::aarch64
{
    uint32_t encode_add_imm(const int rd, const int rn, const int imm12, const bool is64bit, const bool setflags)
    {
        const uint32_t sf = is64bit ? 1 : 0;    /* 64-bit (1) or 32-bit (0) */
        const uint32_t op = 0;                  /* ADD operation (0) */
        const uint32_t S = setflags ? 1 : 0;    /* set flags (ADDS) if true */
        
        return (sf << 31) |         /* size flag */ 
               (op << 30) |         /* operation: 0 for ADD, 1 for SUB */ 
               (S << 29) |          /* set flags */ 
               (0b10001 << 24) |    /* opcode fixed pattern */ 
               (0 << 22) |          /* shift amount */ 
               ((imm12 & 0xFFF) << 10) | /* 12-bit immediate */ 
               (rn << 5) |          /* first source register */ 
               rd;                  /* destination register */
    }

    uint32_t encode_sub_imm(const int rd, const int rn, const int imm12, const bool is64bit, const bool setflags)
    {
        const uint32_t sf = is64bit ? 1 : 0;    /* 64-bit (1) or 32-bit (0) */
        const uint32_t op = 1;                  /* SUB operation (1) */
        const uint32_t S = setflags ? 1 : 0;    /* set flags (SUBS) if true */
        
        return (sf << 31) |         /* size flag */ 
               (op << 30) |         /* operation: 0 for ADD, 1 for SUB */ 
               (S << 29) |          /* set flags */ 
               (0b10001 << 24) |    /* opcode fixed pattern */ 
               (0 << 22) |          /* shift amount */ 
               ((imm12 & 0xFFF) << 10) | /* 12-bit immediate */ 
               (rn << 5) |          /* first source register */ 
               rd;                  /* destination register */
    }

    uint32_t encode_add_reg(const int rd, const int rn, const int rm, const uint32_t shift_type, const uint32_t shift_amount, const bool is64bit, const bool setflags)
    {
        const uint32_t sf = is64bit ? 1 : 0; /* 64-bit (1) or 32-bit (0) */
        constexpr uint32_t op = 0;           /* ADD operation (0) */
        const uint32_t S = setflags ? 1 : 0; /* set flags if true */
        
        /* shift type: 00=LSL, 01=LSR, 10=ASR, 11=Reserved */
        /* shift amount: 6 bits (0-63) */
        if (is64bit == false && shift_amount >= 32)
            /* for 32-bit registers, shift amount must be < 32 */
            return 0; /* error condition */
        
        return (sf << 31) |         /* size flag */
               (op << 30) |         /* operation: 0 for ADD, 1 for SUB */
               (S << 29) |          /* set flags */
               (0b01011 << 24) |    /* opcode fixed pattern */
               (shift_type << 22) | /* shift type */
               (0 << 21) |          /* reserved bit */
               (rm << 16) |         /* second source register */
               ((shift_amount & 0x3F) << 10) | /* shift amount (6 bits) */
               (rn << 5) |          /* first source register */
               rd;                  /* destination register */
    }

    uint32_t encode_sub_reg(const int rd, const int rn, const int rm, const uint32_t shift_type, const uint32_t shift_amount, const bool is64bit, const bool setflags)
    {
        const uint32_t sf = is64bit ? 1 : 0; /* 64-bit (1) or 32-bit (0) */
        constexpr uint32_t op = 1;     /* SUB operation (1) */
        const uint32_t S = setflags ? 1 : 0; /* set flags if true */
        
        /* shift type: 00=LSL, 01=LSR, 10=ASR, 11=Reserved */
        /* shift amount: 6 bits (0-63) */
        if (is64bit == false && shift_amount >= 32) 
            /* for 32-bit registers, shift amount must be < 32 */
            return 0; /* error condition */
        
        return (sf << 31) |         /* size flag */
               (op << 30) |         /* operation: 0 for ADD, 1 for SUB */
               (S << 29) |          /* set flags */
               (0b01011 << 24) |    /* opcode fixed pattern */
               (shift_type << 22) | /* shift type */
               (0 << 21) |          /* reserved bit */
               (rm << 16) |         /* second source register */
               ((shift_amount & 0x3F) << 10) | /* shift amount (6 bits) */
               (rn << 5) |          /* first source register */
               rd;                  /* destination register */
    }

    uint32_t encode_add_ext(const int rd, const int rn, const int rm, const uint32_t option, const uint32_t imm3, const bool is64bit, const bool setflags)
    {
        const uint32_t sf = is64bit ? 1 : 0;    /* 64-bit (1) or 32-bit (0) */
        constexpr uint32_t op = 0;                  /* ADD operation (0) */
        const uint32_t S = setflags ? 1 : 0;    /* set flags if true */
        
        /* option: 000=UXTB, 001=UXTH, 010=UXTW, 011=UXTX, 100=SXTB, 101=SXTH, 110=SXTW, 111=SXTX */
        /* imm3: shift amount for extended register (0-4) */
        if (imm3 > 4)
            return 0; /* error: imm3 must be in range 0-4 */
        
        return (sf << 31) |         /* size flag */
               (op << 30) |         /* operation: 0 for ADD, 1 for SUB */
               (S << 29) |          /* set flags */
               (0b01011001 << 21) | /* opcode fixed pattern */
               (rm << 16) |         /* second source register */
               (option << 13) |     /* extension option */
               (imm3 << 10) |       /* immediate for extension (shift amount) */
               (rn << 5) |          /* first source register */
               rd;                  /* destination register */
    }

    uint32_t encode_sub_ext(const int rd, const int rn, const int rm, const uint32_t option, const uint32_t imm3, const bool is64bit, const bool setflags)
    {
        const uint32_t sf = is64bit ? 1 : 0;    /* 64-bit (1) or 32-bit (0) */
        constexpr uint32_t op = 1;                  /* SUB operation (1) */
        const uint32_t S = setflags ? 1 : 0;    /* set flags if true */
        
        /* option: 000=UXTB, 001=UXTH, 010=UXTW, 011=UXTX, 100=SXTB, 101=SXTH, 110=SXTW, 111=SXTX */
        /* imm3: shift amount for extended register (0-4) */
        if (imm3 > 4)
            return 0; /* error: imm3 must be in range 0-4 */
        
        return (sf << 31) |         /* size flag */
               (op << 30) |         /* operation: 0 for ADD, 1 for SUB */
               (S << 29) |          /* set flags */
               (0b01011001 << 21) | /* opcode fixed pattern */
               (rm << 16) |         /* second source register */
               (option << 13) |     /* extension option */
               (imm3 << 10) |       /* immediate for extension (shift amount) */
               (rn << 5) |          /* first source register */
               rd;                  /* destination register */
    }
}