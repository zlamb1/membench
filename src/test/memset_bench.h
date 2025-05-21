#ifndef MB_MEMSET_BENCH_H
#define MB_MEMSET_BENCH_H 1

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "mem/memset.h"
#include "print.h"

static inline void *
mb_memcmp(void *buf, char c, size_t n)
{
    char *cp = (char *) buf;  
    while (n--) {
        if (*cp++ != c)
            return --cp;
    }
    return NULL; 
}

#define BENCH_MEMSET_HEADER() \
    struct rows rows; \
    mbInitRows(&rows); \
    mbAddColumn(&rows, "Function Name", VALUE_TYPE_STRING); \
    mbAddColumn(&rows, "Iterations", VALUE_TYPE_UINT); \
    mbSetColumnAlign(&rows, COLUMN_ALIGN_RIGHT); \
    mbAddColumn(&rows, "Allocation Size", VALUE_TYPE_UINT); \
    mbSetColumnSuffix(&rows, " BYTES"); \
    mbAddColumn(&rows, "Average Time", VALUE_TYPE_DOUBLE); \
    mbSetColumnAlign(&rows, COLUMN_ALIGN_RIGHT); \
    mbSetColumnSuffix(&rows, "ms"); \
    mbAddColumn(&rows, "Minimum Time", VALUE_TYPE_DOUBLE); \
    mbSetColumnAlign(&rows, COLUMN_ALIGN_RIGHT); \
    mbSetColumnSuffix(&rows, "ms"); \
    mbAddColumn(&rows, "Maximum Time", VALUE_TYPE_DOUBLE); \
    mbSetColumnAlign(&rows, COLUMN_ALIGN_RIGHT); \
    mbSetColumnSuffix(&rows, "ms"); \
    mbAddColumn(&rows, "Memory Throughput", VALUE_TYPE_DOUBLE); \
    mbSetColumnAlign(&rows, COLUMN_ALIGN_RIGHT); \
    mbSetColumnSuffix(&rows, "gb/s");

#define BENCH_MEMSET(F, A) \
    do { \
        unsigned iters = 100, __i = iters; \
        size_t bytes = (A); \
        char c = 0x5F; \
        struct timespec start, end; \
        double gb = bytes / 1000000000.0, elapsed = 0, avg, min = 999999, max = 0, t; \
        while (__i--) { \
            void *tmp; \
            memset(buf, 0x1, bytes); \
            clock_gettime(CLOCK_MONOTONIC, &start); \
            (F)(buf, c, bytes); \
            clock_gettime(CLOCK_MONOTONIC, &end); \
            if ((tmp = mb_memcmp(buf, c, bytes)) != NULL) { \
                printf("%s failed -> expected '%i', got '%i'\n", #F, c, ((int *) tmp)[0]); \
                exit(-1); \
            } \
            t = (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_nsec - start.tv_nsec) / 1000000.0; \
            if (min > t) min = t; \
            if (max < t) max = t; \
            elapsed += t; \
        } \
        avg = elapsed / iters; \
        mbAddRow(&rows); \
        mbAddValue(&rows, createValueString(#F)); \
        mbAddValue(&rows, createValueUInt(iters)); \
        mbAddValue(&rows, createValueUInt(bytes)); \
        mbAddValue(&rows, createValueDouble(avg)); \
        mbAddValue(&rows, createValueDouble(min)); \
        mbAddValue(&rows, createValueDouble(max)); \
        mbAddValue(&rows, createValueDouble(gb / (avg / 1000.0))); \
    } while (0);

#define MB_MAX_ALLOC 20

void
benchMemset(void)
{
    BENCH_MEMSET_HEADER(); 
    void *buf = malloc(1 << MB_MAX_ALLOC); 
    for (unsigned i = 0; i <= MB_MAX_ALLOC; i++) {
        BENCH_MEMSET(mb_memset, 1 << i);
        BENCH_MEMSET(mb_memset_aligned, 1 << i); 
        BENCH_MEMSET(mb_memset_sse2_unaligned, 1 << i);
        BENCH_MEMSET(mb_memset_aligned_sse2_unaligned, 1 << i);
        BENCH_MEMSET(mb_memset_avx2, 1 << i);
        BENCH_MEMSET(memset, 1 << i);
    } 
    free(buf);
    mbPrintRows(&rows);
    mbFreeRows(&rows);
}

#endif
