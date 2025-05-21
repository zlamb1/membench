#ifndef MB_MEMSET32_H
#define MB_MEMSET32_H 1

#include <immintrin.h>
#include <stdlib.h>
#include <xmmintrin.h>

/**
 * memset32_aligned:   assumes 4 byte aligned
 * memset32_unaligned: assumes unalignment
 * <simd>_aligned:     assumes aligned to expected SIMD alignment
 * <simd>_unaligned:   assumes unalignment
 */

static inline void *
mb_memset32_aligned(void *p, int v, size_t n)
{
    int *dp = (int *) p; 
    while (n--)
        *dp++ = v; 
    return p;
}

static inline void *
mb_memset32_aligned_sse2_unaligned(void *p, int v, size_t n)
{
    unsigned cnt = n >> 2; 
    int *ip = (int *) p;
    __m128i vec = _mm_set1_epi32(v); 
    n &= 3;
    while (cnt--) {
        _mm_storeu_si128((__m128i *) ip, vec);
        ip += 4; 
    }
    while (n--)
        *ip++ = v; 
    return p;
}

static inline void *
mb_memset32_aligned_avx_unaligned(void *p, int v, size_t n)
{
    unsigned cnt = n >> 3;
    int *ip = (int *) p;  
    __m256i vec = _mm256_set1_epi32(v);
    n &= 7;  
    while (cnt--) {
        _mm256_storeu_si256((__m256i *) ip, vec);
        ip += 8;
    }
    while (n--)
        *ip++ = v; 
    return p;
}

#endif
