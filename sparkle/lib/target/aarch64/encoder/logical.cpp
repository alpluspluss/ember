#include <cstdint>
#include <sparkle/target/aarch64/common.hpp>
#include <sparkle/target/aarch64/encoding/logical.hpp>

namespace sprk::aarch64
{
    uint32_t encode_and_imm(const int rd, const int rn, const uint64_t imm, const bool is64bit, const bool setflags)
    {
        const uint32_t sf = is64bit ? 1 : 0;    /* 64-bit (1) or 32-bit (0) */
        const uint32_t opc = setflags ? 3 : 0;  /* AND=0, ANDS=3 */
        
        /* process logical immediate */
        uint32_t N, immr, imms;
        if (!encode_logical_imm(imm, is64bit, &N, &immr, &imms)) 
            return 0; /* error: invalid immediate */
        
        return (sf << 31) |         /* size flag */
               (opc << 29) |        /* operation: 0 for AND, 3 for ANDS */
               (0b100100 << 23) |   /* opcode fixed pattern */
               (N << 22) |          /* N bit from logical immediate */
               (immr << 16) |       /* immr from logical immediate */
               (imms << 10) |       /* imms from logical immediate */
               (rn << 5) |          /* source register */
               rd;                  /* destination register */
    }

    uint32_t encode_orr_imm(const int rd, const int rn, const uint64_t imm, const bool is64bit)
    {
        const uint32_t sf = is64bit ? 1 : 0;    /* 64-bit (1) or 32-bit (0) */
        const uint32_t opc = 1;                 /* ORR=1 */
        
        /* process logical immediate */
        uint32_t N, immr, imms;
        if (!encode_logical_imm(imm, is64bit, &N, &immr, &imms))
            return 0; /* error: invalid immediate */
        
        return (sf << 31) |         /* size flag */
               (opc << 29) |        /* operation: 1 for ORR */
               (0b100100 << 23) |   /* opcode fixed pattern */
               (N << 22) |          /* N bit from logical immediate */
               (immr << 16) |       /* immr from logical immediate */
               (imms << 10) |       /* imms from logical immediate */
               (rn << 5) |          /* source register */
               rd;                  /* destination register */
    }

    uint32_t encode_eor_imm(const int rd, const int rn, const uint64_t imm, const bool is64bit)
    {
        const uint32_t sf = is64bit ? 1 : 0;    /* 64-bit (1) or 32-bit (0) */
        constexpr uint32_t opc = 2;                 /* EOR=2 */
        
        /* process logical immediate */
        uint32_t N, immr, imms;
        if (!encode_logical_imm(imm, is64bit, &N, &immr, &imms))
            return 0; /* error: invalid immediate */
        
        return (sf << 31) |         /* size flag */
               (opc << 29) |        /* operation: 2 for EOR */
               (0b100100 << 23) |   /* opcode fixed pattern */
               (N << 22) |          /* N bit from logical immediate */
               (immr << 16) |       /* immr from logical immediate */
               (imms << 10) |       /* imms from logical immediate */
               (rn << 5) |          /* source register */
               rd;                  /* destination register */
    }

    uint32_t encode_and_reg(const int rd, const int rn, int rm, const uint32_t shift_type, const uint32_t shift_amount, const bool is64bit, const bool setflags)
    {
        const uint32_t sf = is64bit ? 1 : 0;        /* 64-bit (1) or 32-bit (0) */
        const uint32_t opc = setflags ? 3 : 0;      /* AND=0, ANDS=3 */
        constexpr uint32_t N = 0;                       /* invert rm (0 for no inversion) */
        
        /* shift type: 00=LSL, 01=LSR, 10=ASR, 11=ROR */
        if (is64bit == false && shift_amount >= 32)
            /* for 32-bit registers, shift amount must be < 32 */
            return 0; /* error condition */
        
        return (sf << 31) |         /* size flag */
               (opc << 29) |        /* operation: 0 for AND, 3 for ANDS */
               (0b01010 << 24) |    /* opcode fixed pattern */
               (shift_type << 22) | /* shift type */
               (N << 21) |          /* N bit (0 for AND, 1 for BIC) */
               (rm << 16) |         /* second source register */
               ((shift_amount & 0x3F) << 10) | /* shift amount (6 bits) */
               (rn << 5) |          /* first source register */
               rd;                  /* destination register */
    }

    uint32_t encode_bic_reg(const int rd, const int rn, int rm, const uint32_t shift_type, const uint32_t shift_amount, const bool is64bit, const bool setflags)
    {
        const uint32_t sf = is64bit ? 1 : 0;        /* 64-bit (1) or 32-bit (0) */
        const uint32_t opc = setflags ? 3 : 0;      /* BIC=0+N, BICS=3+N */
        constexpr uint32_t N = 1;                       /* invert rm (1 for inversion) */
        
        /*  shift type: 00=LSL, 01=LSR, 10=ASR, 11=ROR */
        if (is64bit == false && shift_amount >= 32)
            /* for 32-bit registers, shift amount must be < 32 */
            return 0; /* error condition */
        
        return (sf << 31) |         /* size flag */
               (opc << 29) |        /* operation: 0 for BIC, 3 for BICS */
               (0b01010 << 24) |    /* opcode fixed pattern */
               (shift_type << 22) | /* shift type */
               (N << 21) |          /* N bit (1 for BIC) */
               (rm << 16) |         /* second source register */
               ((shift_amount & 0x3F) << 10) | /* shift amount (6 bits) */
               (rn << 5) |          /* first source register */
               rd;                  /* destination register */
    }

    uint32_t encode_orr_reg(const int rd, const int rn, const int rm, const uint32_t shift_type, const uint32_t shift_amount, const bool is64bit)
    {
        const uint32_t sf = is64bit ? 1 : 0;        /* 64-bit (1) or 32-bit (0) */
        constexpr uint32_t opc = 1;                     /* ORR=1 */
        constexpr uint32_t N = 0;                       /* invert rm (0 for no inversion) */
        
        /* shift type: 00=LSL, 01=LSR, 10=ASR, 11=ROR */
        if (is64bit == false && shift_amount >= 32)
            /* for 32-bit registers, shift amount must be < 32 */
            return 0; /* error condition */
        
        return (sf << 31) |         /* size flag */
               (opc << 29) |        /* operation: 1 for ORR */
               (0b01010 << 24) |    /* opcode fixed pattern */
               (shift_type << 22) | /* shift type */
               (N << 21) |          /* N bit (0 for ORR, 1 for ORN) */
               (rm << 16) |         /* second source register */
               ((shift_amount & 0x3F) << 10) | /* shift amount (6 bits) */
               (rn << 5) |          /* first source register */
               rd;                  /* destination register */
    }

    uint32_t encode_orn_reg(const int rd, const int rn, const int rm, const uint32_t shift_type, const uint32_t shift_amount, const bool is64bit)
    {
        const uint32_t sf = is64bit ? 1 : 0;        /* 64-bit (1) or 32-bit (0) */
        constexpr uint32_t opc = 1;                     /* ORN=1+N */
        constexpr uint32_t N = 1;                       /* invert rm (1 for inversion) */
        
        /* shift type: 00=LSL, 01=LSR, 10=ASR, 11=ROR */
        if (is64bit == false && shift_amount >= 32)
            /* for 32-bit registers, shift amount must be < 32 */
            return 0; /* error condition */
        
        return (sf << 31) |         /* size flag */
               (opc << 29) |        /* operation: 1 for ORN */
               (0b01010 << 24) |    /* opcode fixed pattern */
               (shift_type << 22) | /* shift type */
               (N << 21) |          /* N bit (1 for ORN) */
               (rm << 16) |         /* second source register */
               ((shift_amount & 0x3F) << 10) | /* shift amount (6 bits) */
               (rn << 5) |          /* first source register */
               rd;                  /* destination register */
    }

    uint32_t encode_eor_reg(const int rd, const int rn, const int rm, const uint32_t shift_type, const uint32_t shift_amount, const bool is64bit)
    {
        const uint32_t sf = is64bit ? 1 : 0;        /* 64-bit (1) or 32-bit (0) */
        constexpr uint32_t opc = 2;                     /* EOR=2 */
        constexpr uint32_t N = 0;                       /* invert rm (0 for no inversion) */
        
        /* shift type: 00=LSL, 01=LSR, 10=ASR, 11=ROR */
        if (is64bit == false && shift_amount >= 32)
            /* for 32-bit registers, shift amount must be < 32 */
            return 0; /*  error condition */
        
        return (sf << 31) |         /* size flag */
               (opc << 29) |        /* operation: 2 for EOR */
               (0b01010 << 24) |    /* opcode fixed pattern */
               (shift_type << 22) | /* shift type */
               (N << 21) |          /* N bit (0 for EOR, 1 for EON) */
               (rm << 16) |         /* second source register */
               ((shift_amount & 0x3F) << 10) | /* shift amount (6 bits) */
               (rn << 5) |          /* first source register */
               rd;                  /* destination register */
    }

    uint32_t encode_eon_reg(const int rd, const int rn, const int rm, const uint32_t shift_type, const uint32_t shift_amount, const bool is64bit)
    {
        const uint32_t sf = is64bit ? 1 : 0;        /* 64-bit (1) or 32-bit (0) */
        constexpr uint32_t opc = 2;                     /* EON=2+N */
        constexpr uint32_t N = 1;                       /* invert rm (1 for inversion) */
        
        /* shift type: 00=LSL, 01=LSR, 10=ASR, 11=ROR */
        if (is64bit == false && shift_amount >= 32)
            /* for 32-bit registers, shift amount must be < 32 */
            return 0; /* error condition */
        
        return (sf << 31) |         /* size flag */
               (opc << 29) |        /* operation: 2 for EON */
               (0b01010 << 24) |    /* opcode fixed pattern */
               (shift_type << 22) | /* shift type */
               (N << 21) |          /* N bit (1 for EON) */
               (rm << 16) |         /* second source register */
               ((shift_amount & 0x3F) << 10) | /* shift amount (6 bits) */
               (rn << 5) |          /* first source register */
               rd;                  /* destination register */
    }
}