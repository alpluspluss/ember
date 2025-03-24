#include <sparkle/target/aarch64/encoding/conditional.hpp>

namespace sprk::aarch64
{
    uint32_t encode_csel(const int rd, const int rn, const int rm, const uint32_t cond, const bool is64bit)
    {
        /* CSEL - conditional select */
        const uint32_t sf = is64bit ? 1 : 0;    /* 64-bit (1) or 32-bit (0) */
        constexpr uint32_t op = 0;              /* op = 0 for CSEL */
        constexpr uint32_t o2 = 0;              /* o2 = 0 for CSEL */
        
        if (cond > 15) /* condition code range check */
            return 0;
            
        return (sf << 31) |       /* size flag */
               (op << 30) |       /* operation type */
               (0b011010100 << 21) | /* fixed pattern */
               (rm << 16) |       /* second source register */
               (cond << 12) |     /* condition code */
               (0 << 11) |        /* fixed bit */
               (o2 << 10) |       /* operation variant */
               (rn << 5) |        /* first source register */
               rd;                /* destination register */
    }
    
    uint32_t encode_csinc(const int rd, const int rn, const int rm, const uint32_t cond, const bool is64bit)
    {
        /* CSINC - conditional select increment */
        const uint32_t sf = is64bit ? 1 : 0;    /* 64-bit (1) or 32-bit (0) */
        constexpr uint32_t op = 0;              /* op = 0 for CSINC */
        constexpr uint32_t o2 = 1;              /* o2 = 1 for CSINC */
        
        if (cond > 15) /* condition code range check */
            return 0;  /* error condition */
            
        return (sf << 31) |       /* size flag */
               (op << 30) |       /* operation type */
               (0b011010100 << 21) | /* fixed pattern */
               (rm << 16) |       /* second source register */
               (cond << 12) |     /* condition code */
               (0 << 11) |        /* fixed bit */
               (o2 << 10) |       /* operation variant */
               (rn << 5) |        /* first source register */
               rd;                /* destination register */
    }
    
    uint32_t encode_csinv(const int rd, const int rn, const int rm, const uint32_t cond, const bool is64bit)
    {
        /* CSINV - conditional select invert */
        const uint32_t sf = is64bit ? 1 : 0;    /* 64-bit (1) or 32-bit (0) */
        constexpr uint32_t op = 1;              /* op = 1 for CSINV */
        constexpr uint32_t o2 = 0;              /* o2 = 0 for CSINV */
        
        if (cond > 15) /* Condition code range check */
            return 0;
            
        return (sf << 31) |       /* size flag */
               (op << 30) |       /* operation type */
               (0b011010100 << 21) | /* fixed pattern */
               (rm << 16) |       /* second source register */
               (cond << 12) |     /* condition code */
               (0 << 11) |        /* fixed bit */
               (o2 << 10) |       /* operation variant */
               (rn << 5) |        /* first source register */
               rd;                /* festination register */
    }
    
    uint32_t encode_csneg(const int rd, const int rn, const int rm, const uint32_t cond, const bool is64bit)
    {
        /* CSNEG - conditional select negate */
        const uint32_t sf = is64bit ? 1 : 0;    /* 64-bit (1) or 32-bit (0) */
        constexpr uint32_t op = 1;              /* op = 1 for CSNEG */
        constexpr uint32_t o2 = 1;              /* o2 = 1 for CSNEG */
        
        if (cond > 15) /* condition code range check */
            return 0;
            
        return (sf << 31) |       /* size flag */
               (op << 30) |       /* operation type */
               (0b011010100 << 21) | /* fixed pattern */
               (rm << 16) |       /* second source register */
               (cond << 12) |     /* condition code */
               (0 << 11) |        /* fixed bit */
               (o2 << 10) |       /* operation variant */
               (rn << 5) |        /* first source register */
               rd;                /* destination register */
    }
    
    /* helper for the common CINC pseudoinstruction (conditional Increment) */
    /* CINC is an alias of CSINC with Rm = Rn + inverted condition */
    uint32_t encode_cinc(const int rd, const int rn, const uint32_t cond, const bool is64bit)
    {
        /* CINC Rd, Rn, cond is an alias for CSINC Rd, Rn, Rn, !cond */
        const uint32_t inv_cond = cond ^ 1; /* invert the condition code's lowest bit */
        return encode_csinc(rd, rn, rn, inv_cond, is64bit);
    }
    
    /* helper for the common CINV pseudoinstruction (conditional invert) */
    /* CINV is an alias of CSINV with Rm = Rn + inverted condition */
    uint32_t encode_cinv(const int rd, const int rn, const uint32_t cond, const bool is64bit)
    {
        /* CINV Rd, Rn, cond is an alias for CSINV Rd, Rn, Rn, !cond */
        const uint32_t inv_cond = cond ^ 1; /* invert the condition code's lowest bit */
        return encode_csinv(rd, rn, rn, inv_cond, is64bit);
    }
    
    /* helper for the common CNEG pseudoinstruction (conditional negate) */
    /* CNEG is an alias of CSNEG with Rm = Rn + inverted condition */
    uint32_t encode_cneg(const int rd, const int rn, const uint32_t cond, const bool is64bit)
    {
        /* CNEG Rd, Rn, cond is an alias for CSNEG Rd, Rn, Rn, !cond */
        const uint32_t inv_cond = cond ^ 1; /* invert the condition code's lowest bit */
        return encode_csneg(rd, rn, rn, inv_cond, is64bit);
    }
    
    /* helper for the CSET pseudoinstruction (conditional set) */
    /* CSET is an alias of CSINC with Rn = ZR, Rm = ZR + inverted condition */
    uint32_t encode_cset(const int rd, const uint32_t cond, const bool is64bit)
    {
        /* CSET Rd, cond is an alias for CSINC Rd, ZR, ZR, !cond */
        const uint32_t inv_cond = cond ^ 1; /* invert the condition code's lowest bit */
        constexpr uint32_t ZR = 31;         /* zero register */
        return encode_csinc(rd, ZR, ZR, inv_cond, is64bit);
    }
    
    /* Helper function for the CSETM pseudoinstruction (Conditional Set Minus) */
    /* CSETM is an alias of CSINV with Rn = ZR, Rm = ZR, and inverted condition */
    uint32_t encode_csetm(const int rd, const uint32_t cond, const bool is64bit)
    {
        /* CSETM Rd, cond is an alias for CSINV Rd, ZR, ZR, !cond */
        const uint32_t inv_cond = cond ^ 1; /* invert the condition code's lowest bit */
        constexpr uint32_t ZR = 31;         /* zero register */
        return encode_csinv(rd, ZR, ZR, inv_cond, is64bit);
    }
}