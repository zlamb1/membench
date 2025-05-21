#include <emmintrin.h>
#ifndef MB_MEMSET_H
#define MB_MEMSET_H 1

#include <immintrin.h>
#include <stdlib.h>
#include <xmmintrin.h>

static inline void *
mb_memset(void *p, int v, size_t n)
{
    char c = (char) v;
    char *cp = (char *) p; 
    while (n--)
        *cp++ = c; 
    return p;
}

/* assumes int alignment */
static inline void *
mb_memset_aligned(void *p, int v, size_t n)
{
    /* truncate v */
    char c = (char) v;
    unsigned cnt = n >> 2;
    /* assumes 32-bit ints */
    int *ip = (int *) p, rep = ( (c << 24) | (c << 16) | (c << 8) | c ); 
    n &= 3;  
    while (cnt--)
        *ip++ = rep; 
    char *cp = (char *) ip;
    while (n--)
        *cp++ = c; 
    return p;
}

static inline void *
mb_memset_sse2_unaligned(void *p, int v, size_t n)
{
    char c = v, *cp = (char *) p; 
    unsigned cnt = n >> 4;
    __m128i vec = _mm_set1_epi8((char) c);
    n &= 15; 
    while (cnt--) {
        _mm_storeu_si128((__m128i *) cp, vec);
        cp += 16; 
    }
    while (n--)
        *cp++ = c;
    return p;
}

static inline void *
mb_memset_aligned_sse2_unaligned(void *p, int v, size_t n)
{
    /* truncate */
    char c = v, *cp = (char *) p; 
    /* assumes 32-bit ints */
    int *ip = (int *) p, rep = ( (c << 24) | (c << 16) | (c << 8) | c ); 
    unsigned cnt = n >> 4, cnt2 = (n & 15) >> 2;
    __m128i vec = _mm_set1_epi8((char) c);
    n &= 3; 
    while (cnt--) {
        _mm_storeu_si128((__m128i *) cp, vec);
        cp += 16; 
    }
    ip = (int *) cp;
    while (cnt2--)
        *ip++ = rep; 
    cp = (char *) ip; 
    while (n--)
        *cp++ = c;
    return p;
}

static inline void *
mb_memset_avx2(void *p, int v, size_t n)
{
    char c = v, *cp = (char *) p;
    __m256i vec = _mm256_set1_epi8(c); 
    unsigned cnt = n >> 5;
    n &= 31;
    while (cnt--) {
        _mm256_storeu_si256((__m256i *) cp, vec);
        cp += 32;
    }
    while (n--)
        *cp++ = c; 
    return p; 
}

#endif
