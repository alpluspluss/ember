#include <cstdint>
#include <sparkle/target/aarch64/common.hpp>

namespace sprk::aarch64
{
    bool encode_logical_imm(uint64_t imm, bool is64bit, uint32_t* N, uint32_t* immr, uint32_t* imms)
    {
        *N = 0;
        *immr = 0;
        *imms = 0;
        
        /* special case: all-zeros or all-ones are not encodable */
        if (imm == 0)
            return false;
            
        /* replicate the pattern to 64 bits for 32-bit operations */
        if (!is64bit) 
        {
            imm &= 0xFFFFFFFF;  /* mask to 32 bits */
            
            /* check if it's all ones (not encodable) */
            if (imm == 0xFFFFFFFF)
                return false;
                
            /* replicate to 64 bits for processing */
            imm |= (imm << 32);
        } 
        else 
        {
            /* for 64-bit, check if all ones (not encodable) */
            if (imm == 0xFFFFFFFFFFFFFFFF)
                return false;
        }
        
        /* find rotation pattern */
        int rotation = __builtin_ctz(imm & (imm + 1));
        
        /* rotate right */
        uint64_t normalized = rotation == 0 ? imm : 
            ((imm >> (rotation & 63)) | (imm << (64 - (rotation & 63))));
        
        /* find contiguous ones pattern */
        int leading_zeros = __builtin_clz(normalized);
        int trailing_ones = __builtin_ctz(~normalized);
        int size = leading_zeros + trailing_ones;
        
        /* verify pattern repeats correctly */
        uint64_t rotated_check = (size == 0) ? imm : 
            ((imm >> (size & 63)) | (imm << (64 - (size & 63))));
        if (rotated_check != imm)
            return false;
        
        /* calculate ARM64 encoding parameters */
        *N = (size == 64) ? 1 : 0;
        *immr = (-rotation) & (size - 1);
        *imms = (-(size << 1) | (trailing_ones - 1)) & 0x3F;
        
        return true;
    }
}