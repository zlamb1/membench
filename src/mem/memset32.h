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
    size_t x = 0; 
    __m128i vec = _mm_set1_epi32(v); 
    int *dp = (int *) p;  
    while (x + 4 <= n) {
        _mm_storeu_si128((__m128i *) (dp + x), vec);
        x += 4; 
    }
    while (x < n)
        dp[x++] = v; 
    return p;
}

static inline void *
mb_memset32_aligned_avx_unaligned(void *p, int v, size_t n)
{
    size_t x = 0; 
    __m256i vec = _mm256_set1_epi32(v); 
    int *dp = (int *) p;  
    while (x + 8 <= n) {
        _mm256_storeu_si256((__m256i *) (dp + x), vec);
        x += 8; 
    }
    while (x < n)
        dp[x++] = v; 
    return p;
}

#endif
