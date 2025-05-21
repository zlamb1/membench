#ifndef MB_PRINT_H
#define MB_PRINT_H 1

#include "value.h"

enum 
ColumnAlign
{
    COLUMN_ALIGN_LEFT = 0,
    COLUMN_ALIGN_RIGHT = 1
};

struct 
column
{
    const char *name, *suffix;
    enum ValueType type; 
    enum ColumnAlign align;
};

struct 
row
{
    unsigned nvals; 
    struct value *values;
};

struct 
rows
{
    unsigned ncols;
    struct column *cols;
    unsigned nrows; 
    struct row *rows; 
};

void 
mbInitRows(struct rows *rows); 

int
mbAddColumn(struct rows *rows, const char *name, enum ValueType type); 

void 
mbSetColumnAlign(struct rows *rows, enum ColumnAlign align);

void 
mbSetColumnSuffix(struct rows *rows, const char *suffix); 

int 
mbAddRow(struct rows *rows); 

int 
mbAddValue(struct rows *rows, struct value value); 

int 
mbPrintRows(struct rows *rows);

void
mbFreeRows(struct rows *rows);  

#endif
