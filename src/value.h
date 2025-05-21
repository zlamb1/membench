#ifndef VALUE_H
#define VALUE_H 1

#include <stdarg.h>
#include <stdio.h>

enum 
ValueType
{
    VALUE_TYPE_STRING,
    VALUE_TYPE_UCHAR,
    VALUE_TYPE_CHAR,
    VALUE_TYPE_UINT,
    VALUE_TYPE_INT,
    VALUE_TYPE_FLOAT,
    VALUE_TYPE_DOUBLE
};

union 
value_union
{
    const char *s; 
    unsigned char uc; 
    char c; 
    unsigned u; 
    int i;
    float f; 
    double d; 
};

struct
value
{
    enum ValueType type;
    union value_union v; 
};

static inline struct value
createValueString(const char *str)
{
    return (struct value) {
        .type = VALUE_TYPE_STRING,
        .v = {
            .s = str
        }
    };
}

static inline struct value
createValueUInt(unsigned u)
{
    return (struct value) {
        .type = VALUE_TYPE_UINT,
        .v = {
            .u = u
        }
    };
}

static inline struct value
createValueDouble(double d)
{
    return (struct value) {
        .type = VALUE_TYPE_DOUBLE,
        .v = {
            .d = d
        }
    };
}

static inline char 
getValueFormatSpecifier(enum ValueType type)
{
    switch (type) {
        case VALUE_TYPE_STRING: return 's';
        case VALUE_TYPE_UCHAR:  return 'u';
        case VALUE_TYPE_CHAR:   return 'c';
        case VALUE_TYPE_UINT:   return 'u';
        case VALUE_TYPE_INT:    return 'd';
        case VALUE_TYPE_FLOAT:  return 'f';
        case VALUE_TYPE_DOUBLE: return 'f'; 
        default:                return 0; 
    }
}

static inline int
snprintfValue(struct value value, char *buf, const char *suffix, size_t maxlen, const char *fmt)
{
    switch (value.type) {
        case VALUE_TYPE_STRING:
            return snprintf(buf, maxlen, fmt, value.v.s, suffix);
        case VALUE_TYPE_UCHAR:
            return snprintf(buf, maxlen, fmt, value.v.uc, suffix);
        case VALUE_TYPE_CHAR:
            return snprintf(buf, maxlen, fmt, value.v.c, suffix);
        case VALUE_TYPE_UINT:
            return snprintf(buf, maxlen, fmt, value.v.u, suffix);
        case VALUE_TYPE_INT:
            return snprintf(buf, maxlen, fmt, value.v.i, suffix);
        case VALUE_TYPE_FLOAT:
            return snprintf(buf, maxlen, fmt, value.v.f, suffix);
        case VALUE_TYPE_DOUBLE:
            return snprintf(buf, maxlen, fmt, value.v.d, suffix);
        default:
            return -1;
    }
}

#define PRINTF_VALUE_PREARG1(VALUE, FMT, ARG1) \
    switch ((VALUE).type) { \
        case VALUE_TYPE_STRING: \
            printf((FMT), (ARG1), (VALUE).v.s); break; \
        case VALUE_TYPE_UCHAR: \
            printf((FMT), (ARG1), (VALUE).v.uc); break; \
        case VALUE_TYPE_CHAR: \
            printf((FMT), (ARG1), (VALUE).v.c); break; \
        case VALUE_TYPE_UINT: \
            printf((FMT), (ARG1), (VALUE).v.u); break; \
        case VALUE_TYPE_INT: \
            printf((FMT), (ARG1), (VALUE).v.i); break; \
        case VALUE_TYPE_FLOAT: \
            printf((FMT), (ARG1), (VALUE).v.f); break; \
        case VALUE_TYPE_DOUBLE: \
            printf((FMT), (ARG1), (VALUE).v.d); break; \
    }


#endif
