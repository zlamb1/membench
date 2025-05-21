#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "err.h"
#include "print.h"
#include "value.h"

static const char *UNKNOWN_VALUE_TYPE_MESSAGE = "Unknown Value Type"; 

void 
initPrintRows(struct rows *rows)
{
    assert(rows != NULL); 
    rows->ncols = 0;
    rows->cols = NULL;
    rows->nrows = 0;
    rows->rows = NULL;
}

int
addPrintCol(struct rows *rows, const char *name, enum ValueType type)
{
    assert(rows != NULL); 
    struct column *newCols = realloc(rows->cols, sizeof(struct column) * (++rows->ncols));
    if (newCols == NULL) {
        --rows->ncols;
        return -ERR_ALLOC;
    }
    rows->cols = newCols;
    struct column *col = rows->cols + (rows->ncols - 1);
    col->name = name;
    col->suffix = NULL; 
    col->type = type;
    col->align = COLUMN_ALIGN_LEFT;
    return 0;
}

void 
setColumnAligment(struct rows *rows, enum ColumnAlign align)
{
    assert(rows != NULL);
    assert(rows->ncols && rows->cols != NULL); 
    rows->cols[rows->ncols - 1].align = align; 
}

void 
setColumnSuffix(struct rows *rows, const char *suffix)
{
    assert(rows != NULL);
    assert(rows->ncols && rows->cols != NULL);
    rows->cols[rows->ncols - 1].suffix = suffix; 
}

int 
addPrintRow(struct rows *rows)
{
    assert(rows != NULL); 
    struct row *newRows = realloc(rows->rows, sizeof(struct row) * (++rows->nrows));
    if (newRows == NULL) {
        --rows->nrows;
        return -ERR_ALLOC;
    }
    memset(newRows + (rows->nrows - 1), 0, sizeof(struct row)); 
    rows->rows = newRows;
    return 0;
}

int 
addPrintValue(struct rows *rows, struct value value)
{
    int err; 
    assert(rows != NULL); 
    if (!rows->nrows && (err = addPrintRow(rows)) != 0) {
        return err; 
    }
    struct row *row = (rows->rows + (rows->nrows - 1)); 
    struct value *newValues = realloc(row->values, sizeof(struct value) * (++row->nvals)); 
    if (newValues == NULL) {
        --row->nvals;
        return -ERR_ALLOC;
    }
    row->values = newValues;
    newValues[row->nvals - 1] = value; 
    return 0;
}

int 
benchPrintRows(struct rows *rows)
{
    assert(rows != NULL); 

    if (!rows->ncols)
        return -ERR_INVAID_VALUE;
    
    assert(rows->cols != NULL);

    if (rows->nrows)
        assert(rows->rows != NULL); 

    unsigned maxSizesPerRowBytes = sizeof(unsigned) * rows->ncols, 
            *maxSizesPerRow = malloc(maxSizesPerRowBytes),
             maxSize = 0;

    if (maxSizesPerRow == NULL)
        return -ERR_ALLOC; 

    struct column *cols = rows-> cols; 

    for (unsigned i = 0; i < rows->ncols; ++i) {
        struct column col = cols[i]; 
        maxSizesPerRow[i] = col.name == NULL ? 0 : strlen(col.name);
        if (maxSizesPerRow[i] > maxSize)
            maxSize = maxSizesPerRow[i];  
    }

    for (unsigned r = 0; r < rows->nrows; ++r) {
        struct row *row = rows->rows + r;
        assert(row->nvals >= rows->ncols);  

        for (unsigned v = 0; v < rows->ncols; v++) {
            struct column *col = rows->cols + v; 
            struct value value = row->values[v]; 
            unsigned len = 0, i = 0; 
            int tmp; 
            char fmt[10]; 
            fmt[i++] = '%';
            char c = getValueFormatSpecifier(value.type);
            /* handle unknown types */
            if (c == 0) {
                c = 's'; 
                value.type = VALUE_TYPE_STRING;
                value.v.s = UNKNOWN_VALUE_TYPE_MESSAGE; 
            }
            fmt[i++] = c;
            if (col->suffix != NULL) {
                fmt[i++] = '%';
                fmt[i++] = 's'; 
            }
            fmt[i++] = '\0'; 
            tmp = snprintfValue(value, NULL, col->suffix, 0, fmt); 
            if (tmp < 0)
                len = strlen(UNKNOWN_VALUE_TYPE_MESSAGE); 
            else 
                len = tmp; 
            if (maxSizesPerRow[v] < len)
                maxSizesPerRow[v] = len; 
            if (len > maxSize)
                maxSize = len; 
        }
    }

    /* print header */
    printf("┌");
    for (unsigned i = 0; i < rows->ncols; ++i) {
        for (unsigned j = 0; j < maxSizesPerRow[i]; ++j) {
            printf("─");
        }
        if (i != rows->ncols - 1)
            printf("┬");
    }
    printf("┐\n");

    /* print cols */
    printf("│");
    for (unsigned c = 0; c < rows->ncols; ++c) {
        struct column col = rows->cols[c]; 
        printf("%-*s", maxSizesPerRow[c], col.name);
        if (c != rows->ncols - 1)
            printf("│");
    }
    printf("│\n");

    if (rows->nrows) {
        /* print divider */
        printf("├");
        for (unsigned i = 0; i < rows->ncols; ++i) {
            for (unsigned j = 0; j < maxSizesPerRow[i]; ++j) {
                printf("─");
            }
            if (i != rows->ncols - 1)
                printf("┼");
        }
        printf("┤\n");
    }
    
    /* allocate scratch buffer for handling suffixes */
    char *scratch = malloc(maxSize + 1); 
    if (scratch == NULL) {
        free(maxSizesPerRow); 
        return -ERR_ALLOC;
    } 
    scratch[maxSize] = '\0'; 

    /* print rows */
    for (unsigned r = 0; r < rows->nrows; ++r) {
        struct row *row = rows->rows + r; 

        /* print row */
        printf("│");
        for (unsigned v = 0; v < rows->ncols; ++v) {
            struct column *col = rows->cols + v; 
            struct value value = row->values[v]; 
            unsigned i = 0;
            char fmt[10]; 
            fmt[i++] = '%'; 
            if (col->align == COLUMN_ALIGN_LEFT)
                fmt[i++] = '-'; 
            fmt[i++] = '*';
            if (col->suffix == NULL) {
                char c = getValueFormatSpecifier(value.type); 
                /* handle unknown types */
                if (c == 0) {
                    c = 's'; 
                    value.type = VALUE_TYPE_STRING;
                    value.v.s = UNKNOWN_VALUE_TYPE_MESSAGE; 
                }
                fmt[i++] = c;
            } else {
                /* construct string with value type and suffix */
                fmt[i++] = 's'; 
                char fmt2[5] = { '%', getValueFormatSpecifier(value.type), '%', 's', '\0' };
                /* handle unknown types */
                if (fmt2[1] == 0) {
                    fmt2[1] = 's';
                    value.v.s = UNKNOWN_VALUE_TYPE_MESSAGE; 
                } else {
                    snprintfValue(value, scratch, col->suffix, maxSize, fmt2);
                    value.v.s = scratch; 
                }
                value.type = VALUE_TYPE_STRING;
            }
            fmt[i++] = '\0';
            PRINTF_VALUE_PREARG1(value, fmt, maxSizesPerRow[v]); 
            if (v != rows->ncols - 1) {
                printf("│");
            }
        }
        printf("│\n");

        /* print divider */
        if (r != rows->nrows - 1) {
            printf("├");
            for (unsigned i = 0; i < rows->ncols; ++i) {
                for (unsigned j = 0; j < maxSizesPerRow[i]; ++j) {
                    printf("─");
                }
                if (i != rows->ncols - 1)
                    printf("┼");
            }
            printf("┤\n");
        }
    }

    /* free scratch buffer */
    free(scratch);

    /* print footer */
    printf("└");
    for (unsigned i = 0; i < rows->ncols; ++i) {
        for (unsigned j = 0; j < maxSizesPerRow[i]; ++j) {
            printf("─");
        }
        if (i != rows->ncols - 1)
            printf("┴");
    }
    printf("┘\n");

    free(maxSizesPerRow);

    return 0; 
}

void
freePrintRows(struct rows *rows)
{

    assert(rows != NULL); 
    if (rows->ncols) {
        assert(rows->cols != NULL); 
        free(rows->cols);
    }

    if (rows->nrows) {
        assert(rows->rows != NULL); 

        for (unsigned i = 0; i < rows->nrows; i++) {
            struct row *row = rows->rows + i; 
            free(row->values);
        }

        free(rows->rows); 
    }
}
