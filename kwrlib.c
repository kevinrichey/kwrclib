#include "kwrlib.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>

void_fp swap_fp(void_fp *a, void_fp *b)
{
    void_fp t = *a;
    *a = *b;
    *b = t;
    return *a;
}

//----------------------------------------------------------------------

const char *ErrorCode_String(ErrorCode error)
{
#define X(EnumName)  [ErrorCode_##EnumName] = "ErrorCode_" #EnumName,
    static const char *error_code_strings[] = {
        ERROR_CODE_X_TABLE 
    };
#undef X

    return (error < ErrorCode_Count)? error_code_strings[error]: "Unknown ErrorCode";
}

void Status_Print(Status *stat)
{
    printf("%s: \"%s\" in function %s()\n", ErrorCode_String(stat->error), stat->message, stat->function);
}


bool AssertFailure(const char *message, const char *function)
{
    fprintf(stderr, "%s in function %s()\n", message, function);
    abort();
    return false;
}

_Bool AssertFailureFmt(const char *sourceline, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    fprintf(stderr, "%s Precondition failed: ", sourceline);
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    va_end(args);

    abort();

    return false;
}


//------------------------------------------------------------
//# Memory Management

void *kalloc(void *ptr, size_t size)
{
    return realloc(ptr, size);
}


//------------------------------------------------------------
//# Dynamically Sized Arrays 

void *Dynarray_Alloc(void *a, size_t item_size, size_t num_items)
{
    size_t array_size = item_size * num_items;
    dynarray(char) *na = kalloc(a, sizeof(dynarray(char)) + array_size);
    if (na) {
        na->end = na->begin + array_size;
        if (!a)  na->top = na->begin;
    }

    return na;
}


//------------------------------------------------------------
//# Pseudo-Random Number Generation

static const int XOR_SHIFT_PARAMETERS[][3] = {
   { 1, 3,10}, { 1, 5,16}, { 1, 5,19}, { 1, 9,29}, { 1,11, 6}, { 1,11,16}, { 1,19, 3}, { 1,21,20}, { 1,27,27}, 
   { 2, 5,15}, { 2, 5,21}, { 2, 7, 7}, { 2, 7, 9}, { 2, 7,25}, { 2, 9,15}, { 2,15,17}, { 2,15,25}, { 2,21, 9}, 
   { 3, 1,14}, { 3, 3,26}, { 3, 3,28}, { 3, 3,29}, { 3, 5,20}, { 3, 5,22}, { 3, 5,25}, { 3, 7,29}, { 3,13, 7}, 
   { 3,23,25}, { 3,25,24}, { 3,27,11}, { 4, 3,17}, { 4, 3,27}, { 4, 5,15}, { 5, 3,21}, { 5, 7,22}, { 5, 9,7 }, 
   { 5, 9,28}, { 5, 9,31}, { 5,13, 6}, { 5,15,17}, { 5,17,13}, { 5,21,12}, { 5,27, 8}, { 5,27,21}, { 5,27,25}, 
   { 5,27,28}, { 6, 1,11}, { 6, 3,17}, { 6,17, 9}, { 6,21, 7}, { 6,21,13}, { 7, 1, 9}, { 7, 1,18}, { 7, 1,25}, 
   { 7,13,25}, { 7,17,21}, { 7,25,12}, { 7,25,20}, { 8, 7,23}, { 8,9,23 }, { 9, 5,1 }, { 9, 5,25}, { 9,11,19}, 
   { 9,21,16}, {10, 9,21}, {10, 9,25}, {11, 7,12}, {11, 7,16}, {11,17,13}, {11,21,13}, {12, 9,23}, {13, 3,17}, 
   {13, 3,27}, {13, 5,19}, {13,17,15}, {14, 1,15}, {14,13,15}, {15, 1,29}, {17,15,20}, {17,15,23}, {17,15,26}, 
};

void XorShift_Init(XorShift *state, uint32_t seed, int params_num)
{
    params_num = params_num % array_length(XOR_SHIFT_PARAMETERS);
    const int *params = XOR_SHIFT_PARAMETERS[params_num];

    *state = (XorShift)
    { 
        .a = params[0], 
        .b = params[1], 
        .c = params[2], 
        .x = seed
    };
}

uint32_t XorShift_Rand(XorShift *state)
{
    int x = state->x;
    x ^= (x << state->a);
    x ^= (x >> state->b);
    x ^= (x << state->c);
    return state->x = x;
}

