#include <stdint.h>
#include <stdlib.h>

#define KWRLIB_H_INCLUDED

#define STRINGIFY(x)            #x
#define EXPAND_STRINGIFY(x)     STRINGIFY(x)

#define LINE_STR                EXPAND_STRINGIFY(__LINE__)
#define SOURCE_LINE_STR         __FILE__ ":" LINE_STR ":"

#define COUNT_PARMS2(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _, ...) _
#define COUNT_PARMS(...) COUNT_PARMS2(__VA_ARGS__, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)

#define NOOP       ((void)0)
#define UNUSED(x)  (void)(x)

#define PARAM_0(_0, ...)      _0
#define PARAM_1(_0, _1, ...)  _1

#define STANDARD_ENUM_VALUES(EnumName_) \
  EnumName_##_End,  \
  EnumName_##_Last  = EnumName_##_End - 1, \
  EnumName_##_Count = EnumName_##_End, \
  EnumName_##_First = 0,

#define array_length(array_)  (sizeof(array_) / sizeof(*array_))


typedef void (*void_fp)(void);

void_fp swap_fp(void_fp *a, void_fp *b);



#define ERROR_CODE_X_TABLE \
  X(OK) \
  X(Error) \
  X(Failure) \
  X(Fatal) \
  X(AllocationFailed)

#define X(EnumName)  ErrorCode_##EnumName,
typedef enum {
    ERROR_CODE_X_TABLE
    STANDARD_ENUM_VALUES(ErrorCode)
} ErrorCode;
#undef X

const char *ErrorCode_String(ErrorCode error);

typedef struct Status_struct { 
    ErrorCode   error;
    const char* debug_info;
    const char* function;
    const char* message;
} Status;

#define MakeError(STAT, MSG)     ((Status){ .error=(STAT), .debug_info=SOURCE_LINE_STR, .function=__func__, .message=(MSG) })

void Status_Print(Status *err);

_Bool AssertFailure(const char *message, const char *function);
_Bool AssertFailureFmt(const char *sourceline, const char *format, ...);

#define requires_m(EXPR, msg_)   ((EXPR)? true: AssertFailure(SOURCE_LINE_STR " Precondition failed: \"" msg_ "\"", __func__))

#define requires(EXPR)   ((EXPR)? true: AssertFailure(SOURCE_LINE_STR " Precondition \"" STRINGIFY(EXPR) "\" failed", __func__))
#define ensure(EXPR)    ((EXPR)? NOOP: AssertFailure(SOURCE_LINE_STR " Postcondition \"" STRINGIFY(EXPR) "\" failed", __func__))


//------------------------------------------------------------
//# Memory Management

void *kalloc(void *ptr, size_t size);


//------------------------------------------------------------
//# vector

#define vector_n(TYPE, NAME, ...)   \
  union { \
      struct { TYPE __VA_ARGS__; }; \
      TYPE NAME[COUNT_PARMS(__VA_ARGS__)]; \
  }

#define vector(TYPE, ...)   vector_n(TYPE, components, __VA_ARGS__)
#define vec_length(vec_)    (size_t)(array_length((vec_).components))

//------------------------------------------------------------
//# Dynamically Sized Arrays 

#define dynarray(TYPE)  struct {  TYPE *top; TYPE *end; TYPE begin[]; }

void  *Dynarray_Alloc(void *a, size_t item_size, size_t num_items);

#define length(da_)     (size_t)((da_)->top - (da_)->begin)
#define capacity(da_)   (size_t)((da_)->end - (da_)->begin)
#define remaining(da_)  (size_t)((da_)->end - (da_)->top)
#define is_empty(da_)   (_Bool)((da_)->top == (da_)->begin)
#define is_full(da_)    (_Bool)((da_)->top == (da_)->end)
#define da_get(da_, at_)   (requires(at_ < length(da_)), (da_)->begin[(at_)])
#define da_set(da_, at_, val_)   (requires((at_) < length(da_)), (da_)->begin[(at_)] = (val_))
#define push(da_, val_)       (*(da_)->top++ = (val_))

#ifndef ARRAY_DEFAULT_SIZE  
#define    ARRAY_DEFAULT_SIZE  64
#endif 

#define new_dynarray(...)                    Dynarray_Alloc(NULL, sizeof(PARAM_0(__VA_ARGS__)), PARAM_1(__VA_ARGS__, ARRAY_DEFAULT_SIZE))
#define enlarge_x(dynarray_, growth_, ...)   Dynarray_Alloc((dynarray_), sizeof(*(dynarray_)->begin), capacity(dynarray_) + (growth_))
#define enlarge(...)                         enlarge_x(__VA_ARGS__, capacity(PARAM_0(__VA_ARGS__)))



//------------------------------------------------------------
//# Pseudo-Random Number Generation

typedef struct XorShiftState {
    int a, b, c;
    uint32_t x;
} XorShift;

void XorShift_Init(XorShift *state, uint32_t seed, int params_num);
uint32_t XorShift_Rand(XorShift *state);

