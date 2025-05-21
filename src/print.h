#ifndef PRINT_H
#define PRINT_H 1

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
initPrintRows(struct rows *rows); 

int
addPrintCol(struct rows *rows, const char *name, enum ValueType type); 

void 
setColumnAligment(struct rows *rows, enum ColumnAlign align);

void 
setColumnSuffix(struct rows *rows, const char *suffix); 

int 
addPrintRow(struct rows *rows); 

int 
addPrintValue(struct rows *rows, struct value value); 

int 
benchPrintRows(struct rows *rows);

void
freePrintRows(struct rows *rows);  

#endif
