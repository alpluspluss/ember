#include <sparkle/target/aarch64/encoding/mul.hpp>

namespace sprk::aarch64
{
    uint32_t encode_mul(const int rd, const int rn, const int rm, const bool is64bit)
    {
        /* MUL is an alias for MADD with RA=XZR */
        constexpr uint32_t ra = 31; /* zero register (XZR/WZR) */
        return encode_madd(rd, rn, rm, ra, is64bit);
    }
    
    uint32_t encode_madd(const int rd, const int rn, const int rm, const int ra, const bool is64bit)
    {
        /* MADD - Multiply-Add: Rd = Rn * Rm + Ra */
        const uint32_t sf = is64bit ? 1 : 0;    /* 64-bit (1) or 32-bit (0) */
        constexpr uint32_t o0 = 0;              /* o0 = 0 for MADD operation */
        
        return (sf << 31) |        /* size flag */
               (0 << 30) |         /* fixed bit */
               (0b011011 << 24) |  /* fixed pattern */
               (0 << 23) |         /* fixed bit for non-extended ops */
               (rm << 16) |        /* second source register (multiplier) */
               (o0 << 15) |        /* operation type: 0 for MADD */
               (ra << 10) |        /* third source register (addend) */
               (rn << 5) |         /* first source register (multiplier) */
               rd;                 /* destination register */
    }
    
    uint32_t encode_msub(const int rd, const int rn, const int rm, const int ra, const bool is64bit)
    {
        /* MSUB - Multiply-Subtract: Rd = Ra - Rn * Rm */
        const uint32_t sf = is64bit ? 1 : 0;    /* 64-bit (1) or 32-bit (0) */
        constexpr uint32_t o0 = 1;              /* o0 = 1 for MSUB operation */
        
        return (sf << 31) |        /* size flag */
               (0 << 30) |         /* fixed bit */
               (0b011011 << 24) |  /* fixed pattern */
               (0 << 23) |         /* fixed bit for non-extended ops */
               (rm << 16) |        /* second source register (multiplier) */
               (o0 << 15) |        /* operation type: 1 for MSUB */
               (ra << 10) |        /* third source register (subtrahend) */
               (rn << 5) |         /* first source register (multiplier) */
               rd;                 /* destination register */
    }
    
    uint32_t encode_mneg(const int rd, const int rn, const int rm, const bool is64bit)
    {
        /* MNEG is an alias for MSUB with RA=XZR */
        constexpr uint32_t ra = 31; /* zero register (XZR/WZR) */
        return encode_msub(rd, rn, rm, ra, is64bit);
    }
    
    uint32_t encode_smaddl(const int rd, const int rn, const int rm, const int ra)
    {
        /* SMADDL - Signed Multiply-Add Long: Rd = Rn * Rm + Ra (sign-extended) */
        constexpr uint32_t sf = 1;    /* always 64-bit (1) */
        constexpr uint32_t o0 = 0;    /* o0 = 0 for MADD operation */
        constexpr uint32_t U = 0;     /* U = 0 for signed operation */
        
        return (sf << 31) |        /* size flag */
            (0 << 30) |         /* fixed bit */
            (0b011011 << 24) |  /* fixed pattern */
            (U << 23) |         /* U bit: 0 for signed */
            (0b01 << 21) |      /* fixed pattern "01" for MULL operations */
            (rm << 16) |        /* second source register (multiplier) */
            (o0 << 15) |        /* operation type: 0 for MADD */
            (ra << 10) |        /* third source register (addend) */
            (rn << 5) |         /* first source register (multiplier) */
            rd;                 /* destination register */
    }
    
    uint32_t encode_smsubl(const int rd, const int rn, const int rm, const int ra)
    {
        /* SMSUBL - Signed Multiply-Subtract Long: Rd = Ra - Rn * Rm (sign-extended) */
        constexpr uint32_t sf = 1;    /* always 64-bit (1) */
        constexpr uint32_t o0 = 1;    /* o0 = 1 for MSUB operation */
        constexpr uint32_t U = 0;     /* U = 0 for signed operation */
        
        return (sf << 31) |        /* size flag */
               (0 << 30) |         /* fixed bit */
               (0b011011 << 24) |  /* fixed pattern */
               (U << 23) |         /* U bit: 0 for signed */
               (0b01 << 21) |      /* fixed bit for extended ops */
               (rm << 16) |        /* second source register (multiplier) */
               (o0 << 15) |        /* operation type: 1 for MSUB */
               (ra << 10) |        /* third source register (subtrahend) */
               (rn << 5) |         /* first source register (multiplier) */
               rd;                 /* destination register */
    }
    
    uint32_t encode_smull(const int rd, const int rn, const int rm)
    {
        /* SMULL is an alias for SMADDL with RA=XZR */
        constexpr uint32_t ra = 31; // Zero register (XZR)
        return encode_smaddl(rd, rn, rm, ra);
    }
    
    uint32_t encode_smnegl(const int rd, const int rn, const int rm)
    {
        /* SMNEGL is an alias for SMSUBL with RA=XZR */
        constexpr uint32_t ra = 31; /* zero register (XZR/WZR) */
        return encode_smsubl(rd, rn, rm, ra);
    }
    
    uint32_t encode_umaddl(const int rd, const int rn, const int rm, const int ra)
    {
        /* UMADDL - unsigned multiply-add long: Rd = Rn * Rm + Ra (zero-extended) */
        constexpr uint32_t sf = 1;    /* always 64-bit (1) */
        constexpr uint32_t o0 = 0;    /* o0 = 0 for MADD operation */
        constexpr uint32_t U = 1;     /* U = 1 for unsigned operation */
        
        return (sf << 31) |        /* size flag */
               (0 << 30) |         /* fixed bit */
               (0b011011 << 24) |  /* fixed pattern */
               (U << 23) |         /* U bit: 1 for unsigned */
               (0b01 << 21) |      /* fixed bit for extended ops */
               (rm << 16) |        /* second source register (multiplier) */
               (o0 << 15) |        /* operation type: 0 for MADD */
               (ra << 10) |        /* third source register (addend) */
               (rn << 5) |         /* first source register (multiplier) */
               rd;                 /* destination register */
    }
    
    uint32_t encode_umsubl(const int rd, const int rn, const int rm, const int ra)
    {
        /* UMSUBL - unsigned multiply-subtract long: Rd = Ra - Rn * Rm (zero-extended) */
        constexpr uint32_t sf = 1;    /* always 64-bit (1) */
        constexpr uint32_t o0 = 1;    /* o0 = 1 for MSUB operation */
        constexpr uint32_t U = 1;     /* U = 1 for unsigned operation */
        
        return (sf << 31) |        /* size flag */
               (0 << 30) |         /* fixed bit */
               (0b011011 << 24) |  /* fixed pattern */
               (U << 23) |         /* U bit: 1 for unsigned */
               (0b01 << 21) |         /* fixed bit for extended ops */
               (rm << 16) |        /* second source register (multiplier) */
               (o0 << 15) |        /* operation type: 1 for MSUB */
               (ra << 10) |        /* third source register (subtrahend) */
               (rn << 5) |         /* first source register (multiplier) */
               rd;                 /* destination register */
    }
    
    uint32_t encode_umull(const int rd, const int rn, const int rm)
    {
        /* UMULL is an alias for UMADDL with RA=XZR */
        constexpr uint32_t ra = 31; /* zero register (XZR/WZR) */
        return encode_umaddl(rd, rn, rm, ra);
    }
    
    uint32_t encode_umnegl(const int rd, const int rn, const int rm)
    {
        /* UMNEGL is an alias for UMSUBL with RA=XZR */
        constexpr uint32_t ra = 31; /* zero register (XZR/WZR) */
        return encode_umsubl(rd, rn, rm, ra);
    }
}