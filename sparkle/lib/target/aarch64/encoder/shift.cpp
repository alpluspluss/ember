#include <sparkle/target/aarch64/encoding/shift.hpp>

namespace sprk::aarch64
{
    uint32_t encode_lslv(const int rd, const int rn, const int rm, const bool is64bit)
    {
        const uint32_t sf = is64bit ? 1 : 0;    /* 64-bit (1) or 32-bit (0) */
        constexpr uint32_t op2 = 0;                 /* LSL=0 */
        
        return (sf << 31) |         /* size flag */
               (0b0011010110 << 21) | /* opcode fixed pattern */
               (rm << 16) |         /* shift amount register */
               (0b0010 << 12) |     /* fixed pattern */
               (op2 << 10) |        /* operation: 0 for LSL */
               (rn << 5) |          /* source register */
               rd;                  /* destination register */
    }

    uint32_t encode_lsrv(const int rd, const int rn, const int rm, const bool is64bit)
    {
        const uint32_t sf = is64bit ? 1 : 0;    /* 64-bit (1) or 32-bit (0) */
        constexpr uint32_t op2 = 1;                 /* LSR=1 */
        
        return (sf << 31) |         /* size flag */
               (0b0011010110 << 21) | /* opcode fixed pattern */
               (rm << 16) |         /* shift amount register */
               (0b0010 << 12) |     /* fixed pattern */
               (op2 << 10) |        /* operation: 1 for LSR */
               (rn << 5) |          /* source register */
               rd;                  /* destination register */
    }

    uint32_t encode_asrv(const int rd, const int rn, const int rm, const bool is64bit)
    {
        const uint32_t sf = is64bit ? 1 : 0;    /* 64-bit (1) or 32-bit (0) */
        constexpr uint32_t op2 = 2;                 /* ASR=2 */
        
        return (sf << 31) |         /* size flag */
               (0b0011010110 << 21) | /* opcode fixed pattern */
               (rm << 16) |         /* shift amount register */
               (0b0010 << 12) |     /* fixed pattern */
               (op2 << 10) |        /* operation: 2 for ASR */
               (rn << 5) |          /* source register */
               rd;                  /* destination register */
    }

    uint32_t encode_rorv(const int rd, const int rn, const int rm, const bool is64bit)
    {
        const uint32_t sf = is64bit ? 1 : 0;    /* 64-bit (1) or 32-bit (0) */
        constexpr uint32_t op2 = 3;                 /* ROR=3 */
        
        return (sf << 31) |         /* size flag */
               (0b0011010110 << 21) | /* opcode fixed pattern */
               (rm << 16) |         /* shift amount register */
               (0b0010 << 12) |     /* fixed pattern */
               (op2 << 10) |        /* operation: 3 for ROR */
               (rn << 5) |          /* source register */
               rd;                  /* destination register */
    }

    uint32_t encode_lsl_imm(const int rd, const int rn, const uint32_t shift, const bool is64bit)
    {
        const uint32_t sf = is64bit ? 1 : 0;    /* 64-bit (1) or 32-bit (0) */
        const uint32_t N = sf;                  /* must match sf */
        constexpr uint32_t opc = 2;                 /* UBFM=2 (unsigned bit field move) */

        /* LSL is an alias of UBFM with specific encoding for immr and imms */
        const uint32_t reg_size = is64bit ? 64 : 32;
        
        /* for LSL, immr = (-shift) % reg_size, imms = reg_size - 1 - shift */
        const uint32_t immr = (reg_size - shift) % reg_size;
        const uint32_t imms = reg_size - 1 - shift;
        
        /* check for valid shift amount */
        if (shift >= reg_size)
            return 0; /* error: shift too large */
        
        return (sf << 31) |         /* size flag */
               (opc << 29) |        /* operation: 2 for UBFM */
               (0b100110 << 23) |   /* opcode fixed pattern */
               (N << 22) |          /* N bit must match sf */
               (immr << 16) |       /* immr field */
               (imms << 10) |       /* imms field */
               (rn << 5) |          /* source register */
               rd;                  /* destination register */
    }

    uint32_t encode_lsr_imm(const int rd, const int rn, const uint32_t shift, const bool is64bit)
    {
        const uint32_t sf = is64bit ? 1 : 0;    /* 64-bit (1) or 32-bit (0) */
        const uint32_t N = sf;                  /* must match sf */
        constexpr uint32_t opc = 2;                 /* UBFM=2 (unsigned bit field move) */
        
        /* LSR is an alias of UBFM with specific encoding */
        const uint32_t reg_size = is64bit ? 64 : 32;
        
        /* for LSR, immr = shift, imms = reg_size - 1 */
        const uint32_t immr = shift;
        const uint32_t imms = reg_size - 1;
        
        /* check for valid shift amount */
        if (shift >= reg_size)
            return 0; /* error: shift too large */
        
        return (sf << 31) |         /* size flag */
               (opc << 29) |        /* operation: 2 for UBFM */
               (0b100110 << 23) |   /* opcode fixed pattern */
               (N << 22) |          /* N bit must match sf */
               (immr << 16) |       /* immr field */
               (imms << 10) |       /* imms field */
               (rn << 5) |          /* source register */
               rd;                  /* destination register */
    }

    uint32_t encode_asr_imm(int rd, int rn, uint32_t shift, bool is64bit)
    {
        const uint32_t sf = is64bit ? 1 : 0;    /* 64-bit (1) or 32-bit (0) */
        const uint32_t N = sf;                  /* must match sf */
        constexpr uint32_t opc = 0;                 /* SBFM=0 (signed bit field move) */
        
        /* ASR is an alias of SBFM with specific encoding */
        const uint32_t reg_size = is64bit ? 64 : 32;
        
        /* for ASR, immr = shift, imms = reg_size - 1 */
        const uint32_t immr = shift;
        const uint32_t imms = reg_size - 1;
        
        /* check for valid shift amount */
        if (shift >= reg_size)
            return 0; /* error: shift too large */
        
        return (sf << 31) |         /* size flag */
               (opc << 29) |        /* operation: 0 for SBFM */
               (0b100110 << 23) |   /* opcode fixed pattern */
               (N << 22) |          /* N bit must match sf */
               (immr << 16) |       /* immr field */
               (imms << 10) |       /* imms field */
               (rn << 5) |          /* source register */
               rd;                  /* destination register */
    }

    uint32_t encode_ror_imm(const int rd, const int rn, const uint32_t shift, const bool is64bit)
    {
        const uint32_t sf = is64bit ? 1 : 0;    /* 64-bit (1) or 32-bit (0) */
        const uint32_t N = sf;                  /* must match sf */
        const uint32_t rm = rn;                 /* for ROR, both sources are the same */
        
        /* ROR is an alias of EXTR when both source registers are the same */
        const uint32_t reg_size = is64bit ? 64 : 32;
        
        /* for EXTR, imms = shift */
        const uint32_t imms = shift;

        /* check for valid shift amount */
        if (shift >= reg_size)
            return 0; /* error: shift too large */

        return (sf << 31) |         /* size flag */
               (0b00100111 << 23) | /* opcode fixed pattern */
               (N << 22) |          /* N bit must match sf */
               (0 << 21) |          /* o0 bit */
               (rm << 16) |         /* second source register (same as first) */
               (imms << 10) |       /* imms field (shift amount) */
               (rn << 5) |          /* first source register */
               rd;                  /* destination register */
    }
}