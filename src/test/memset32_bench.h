#ifndef MEMSET32_BENCH_H
#define MEMSET32_BENCH_H 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "memset32.h"
#include "print.h"

static inline void * 
memcmp32(void *buf, int v, size_t n)
{
    int *dp = (int *) buf; 
    while (n--)
        if (*dp++ != v)
            return --dp;  
    return NULL;
}

#define BENCH_MEMSET32_HEADER() \
    struct rows rows; \
    initPrintRows(&rows); \
    addPrintCol(&rows, "Function Name", VALUE_TYPE_STRING); \
    addPrintCol(&rows, "Iterations", VALUE_TYPE_UINT); \
    setColumnAligment(&rows, COLUMN_ALIGN_RIGHT); \
    addPrintCol(&rows, "Allocation Size", VALUE_TYPE_UINT); \
    setColumnSuffix(&rows, " BYTES"); \
    addPrintCol(&rows, "Average Time", VALUE_TYPE_DOUBLE); \
    setColumnAligment(&rows, COLUMN_ALIGN_RIGHT); \
    setColumnSuffix(&rows, "ms"); \
    addPrintCol(&rows, "Minimum Time", VALUE_TYPE_DOUBLE); \
    setColumnAligment(&rows, COLUMN_ALIGN_RIGHT); \
    setColumnSuffix(&rows, "ms"); \
    addPrintCol(&rows, "Maximum Time", VALUE_TYPE_DOUBLE); \
    setColumnAligment(&rows, COLUMN_ALIGN_RIGHT); \
    setColumnSuffix(&rows, "ms"); \
    addPrintCol(&rows, "Memory Throughput", VALUE_TYPE_DOUBLE); \
    setColumnAligment(&rows, COLUMN_ALIGN_RIGHT); \
    setColumnSuffix(&rows, "gb/s");

#define BENCH_MEMSET32(F, A) \
    do { \
        unsigned iters = 1000, __i = iters; \
        size_t size = (A), bytes = size * 4; \
        int v = 0xAABBCCDD; \
        struct timespec start, end; \
        double gb = bytes / 1000000000.0, elapsed = 0, avg, min = 999999, max = 0, t; \
        while (__i--) { \
            void *buf = malloc(bytes), *tmp; \
            memset(buf, 0, bytes); \
            clock_gettime(CLOCK_MONOTONIC, &start); \
            (F)(buf, v, size); \
            clock_gettime(CLOCK_MONOTONIC, &end); \
            if ((tmp = memcmp32(buf, v, size)) != NULL) { \
                printf("%s failed -> expected '%i', got '%i'\n", #F, v, ((int *) tmp)[0]); \
                exit(-1); \
            } \
            t = (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_nsec - start.tv_nsec) / 1000000.0; \
            if (min > t) min = t; \
            if (max < t) max = t; \
            elapsed += t; \
            free(buf); \
        } \
        avg = elapsed / iters; \
        addPrintRow(&rows); \
        addPrintValue(&rows, createValueString(#F)); \
        addPrintValue(&rows, createValueUInt(iters)); \
        addPrintValue(&rows, createValueUInt(bytes)); \
        addPrintValue(&rows, createValueDouble(avg)); \
        addPrintValue(&rows, createValueDouble(min)); \
        addPrintValue(&rows, createValueDouble(max)); \
        addPrintValue(&rows, createValueDouble(gb / (avg / 1000.0))); \
    } while (0);

void
benchMemset32(void)
{
    BENCH_MEMSET32_HEADER(); 
    for (unsigned i = 0; i <= 18; i++) {
        BENCH_MEMSET32(memset32_aligned, 1 << i);
        BENCH_MEMSET32(memset32_aligned_sse2_unaligned, 1 << i); 
        BENCH_MEMSET32(memset32_aligned_avx_unaligned, 1 << i); 
    } 
    benchPrintRows(&rows);
    freePrintRows(&rows);
}

#endif
