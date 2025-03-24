#include <sparkle/target/aarch64/encoding/bitmanip.hpp>

namespace sprk::aarch64
{
    uint32_t encode_rbit(const int rd, const int rn, const bool is64bit)
    {
        /* RBIT - reverse bits: reverses the bit order in a register */
        const uint32_t sf = is64bit ? 1 : 0;    /* 64-bit (1) or 32-bit (0) */
        constexpr uint32_t opc = 0;             /* opc = 0 for RBIT */
        
        return (sf << 31) |        /* size flag */
               (0b101101011 << 22) | /* fixed pattern */
               (0b0000000000 << 12) | /* fixed bits */
               (opc << 10) |       /* operation code */
               (rn << 5) |         /* source register */
               rd;                 /* destination register */
    }
    
    uint32_t encode_rev16(const int rd, const int rn, const bool is64bit)
    {
        /* REV16 - reverse bytes in 16-bit halfwords */
        const uint32_t sf = is64bit ? 1 : 0;    /* 64-bit (1) or 32-bit (0) */
        constexpr uint32_t opc = 1;             /* opc = 1 for REV16 */
        
        return (sf << 31) |        /* size flag */
               (0b101101011 << 22) | /* fixed pattern */
               (0b0000000000 << 12) | /* fixed bits */
               (opc << 10) |       /* operation code */
               (rn << 5) |         /* source register */
               rd;                 /* destination register */
    }
    
    uint32_t encode_rev32(const int rd, const int rn, const bool is64bit)
    {
        /* REV32 - reverse bytes in 32-bit words */
        /* @note: only valid for 64-bit registers (sf=1) */
        if (!is64bit)
            return 0;  /* error: REV32 only valid for 64-bit registers */
            
        constexpr uint32_t sf = 1;              /* always 64-bit (1) */
        constexpr uint32_t opc = 2;             /* opc = 2 for REV32 */
        
        return (sf << 31) |        /* size flag */
               (0b101101011 << 22) | /* fixed pattern */
               (0b0000000000 << 12) | /* fixed bits */
               (opc << 10) |       /* operation code */
               (rn << 5) |         /* source register */
               rd;                 /* destination register */
    }
    
    uint32_t encode_rev(const int rd, const int rn, const bool is64bit)
    {
        /* REV - Reverse bytes:
           - For 32-bit (sf=0): reverses bytes in the entire 32-bit word
           - For 64-bit (sf=1): reverses bytes in the entire 64-bit doubleword (REV64 alias) */
           
        const uint32_t sf = is64bit ? 1 : 0;    /* 64-bit (1) or 32-bit (0) */
        const uint32_t opc = is64bit ? 3 : 2;   /* opc = 2 for 32-bit REV, 3 for 64-bit REV */
        
        return (sf << 31) |        /* size flag */
               (0b101101011 << 22) | /* fixed pattern */
               (0b0000000000 << 12) | /* fixed bits */
               (opc << 10) |       /* operation code */
               (rn << 5) |         /* source register */
               rd;                 /* destination register */
    }
    
    // uint32_t encode_clz(const int rd, const int rn, const bool is64bit)
    // {
    //     /* CLZ - count leading zeros: counts the number of leading zeros in a register */
    //     const uint32_t sf = is64bit ? 1 : 0;    /* 64-bit (1) or 32-bit (0) */
    //     constexpr uint32_t opcode = 0b100;    /* opcode = 00100 for CLZ */
        
    //     return (sf << 31) |        /* size flag */
    //            (0b101101011000000 << 15) | /* fixed pattern */
    //            (opcode << 10) |    /* operation code */
    //            (rn << 5) |         /* source register */
    //            rd;                 /* destination register */
    // }
    
    // uint32_t encode_cls(const int rd, const int rn, const bool is64bit)
    // {
    //     /* CLS - count leading sign bits: counts the number of consecutive bits 
    //        that match the sign bit, starting from the most significant bit */
    //     const uint32_t sf = is64bit ? 1 : 0;    /* 64-bit (1) or 32-bit (0) */
    //     constexpr uint32_t opcode = 0b101;    /* opcode = 00101 for CLS */
        
    //     return (sf << 31) |        /* size flag */
    //            (0b101101011000000 << 15) | /* fixed pattern */
    //            (opcode << 10) |    /* operation code */
    //            (rn << 5) |         /* source register */
    //            rd;                 /* destination register */
    // }
    
    // /* ARM64 also has a count trailing zeros instruction in newer revisions */
    // uint32_t encode_ctz(const int rd, const int rn, const bool is64bit)
    // {
    //     /* CTZ - count trailing zeros: counts the number of trailing zeros in a register */
    //     /* @note: this is available in ARMv8.5-A and later */
    //     const uint32_t sf = is64bit ? 1 : 0;    /* 64-bit (1) or 32-bit (0) */
    //     constexpr uint32_t opcode = 0b110;    /* opcode = 00110 for CTZ */
        
    //     return (sf << 31) |        /* size flag */
    //            (0b101101011000000 << 15) | /* fixed pattern */
    //            (opcode << 10) |    /* operation code */
    //            (rn << 5) |         /* source register */
    //            rd;                 /* destination register */
    // }
    
    // /* helper function for ARMv8.3-A and later extensions: Count Non-Zero Bytes */
    // uint32_t encode_cnt(const int rd, const int rn, const bool is64bit)
    // {
    //     /* CNT - count set bits: counts the number of 1 bits in a register */
    //     /* @note: this is available in ARMv8.3-A and later */
    //     const uint32_t sf = is64bit ? 1 : 0;    /* 64-bit (1) or 32-bit (0) */
    //     constexpr uint32_t opcode = 0b111;    /* opcode = 00111 for CNT */
        
    //     return (sf << 31) |        /* size flag */
    //            (0b101101011000000 << 15) | /* fixed pattern */
    //            (opcode << 10) |    /* operation code */
    //            (rn << 5) |         /* source register */
    //            rd;                 /* destination register */
    // }
}