#include <cstdint>
#include <sparkle/target/aarch64/common.hpp>

namespace sprk::aarch64
{
    bool encode_logical_imm(uint64_t imm, bool is64bit, uint32_t* N, uint32_t* immr, uint32_t* imms)
    {
        /* the immediate encoding process for logical operations is complex */
        /* and involves finding a pattern that can be represented as a bitmask */
        
        /* init default values */
        *N = 0;
        *immr = 0;
        *imms = 0;
        
        /* qcheck for all-zeros or all-ones (not encodable as logical immediates) */
        if (imm == 0 || (is64bit && imm == 0xFFFFFFFFFFFFFFFF) || (!is64bit && imm == 0xFFFFFFFF))
            return false;
        
        /* limit check for 32-bit operations (top 32 bits must be all 0 or all 1) */
        if (!is64bit) 
        {
            imm &= 0xFFFFFFFF; /* mask to 32 bits */
            imm |= (imm << 32); /* replicate to top 32 bits */
        }
        
        /* find the element size (e) and rotation (r) */
        int e = 64;
        while (e >= 2) 
        {
            if (is_pattern_repeating(imm, e)) 
                break;
            e >>= 1; /* go down every pow2: 64 -> 32 -> 16 etc */
        }
        
        if (e < 2) /* e should be zero */
            return false; /* not representable as logical immediate */
        
        /* find runs of 1s in the base pattern */
        uint64_t pattern = imm & ((1ULL << e) - 1);
        int s, r;
        
        if (!find_s_r(pattern, e, &s, &r))
            return false; /* not encodable */
        
        /* generate fields for the instruction */
        *N = (e == 64) ? 1 : 0;
        *immr = r & ((1 << log2(e)) - 1);
        *imms = s & ((1 << log2(e)) - 1);
        
        return true;
    }

    /* check if a pattern repeats every 'n' bits */
    bool is_pattern_repeating(uint64_t imm, int e)
    {
        uint64_t pattern = imm & ((1ULL << e) - 1);
    
        for (int i = e; i < 64; i += e) 
        {
            uint64_t slice = (imm >> i) & ((1ULL << e) - 1);
            if (slice != pattern)
                return false;
        }
        
        return true;
    }

    bool find_s_r(uint64_t pattern, int e, int* s, int* r)
    {
        int max_ones = 0;
        int max_r = 0;
        
        for (int r = 0; r < e; r++) 
        {
            uint64_t rotated = ((pattern >> r) | (pattern << (e - r))) & ((1ULL << e) - 1);

            int ones = ctones(rotated);
            if (ones > max_ones) 
            {
                max_ones = ones;
                max_r = r;
            }
        }
        
        *r = max_r;
        *s = max_ones - 1;
        
        /* check if this pattern is encodable */
        return (reconstruct_imm(*r, *s, e) == pattern);
    }

    int ctones(uint64_t value) /* to count trailing 1s in a pattern */
    {
        int count = 0;
    
        while (value & 1) 
        {
            count++;
            value >>= 1;
        }
        
        return count;
    }

    /* helper function to reconstruct an immediate from r, s, and e */
    uint64_t reconstruct_imm(int r, int s, int e)
    {
        uint64_t mask = (1ULL << (s + 1)) - 1;
        return ((mask >> r) | (mask << (e - r))) & ((1ULL << e) - 1);
    }
    
    int log2(int n) /* helper function to compute log2 of a power of 2 */
    {
        int result = 0;
        while (n > 1) 
        {
            n >>= 1;
            result++;
        }
        return result;
    }
}