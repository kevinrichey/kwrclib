#define KWRLIB_H_INCLUDED

#define STRINGIFY(x)            #x
#define EXPAND_STRINGIFY(x)     STRINGIFY(x)

#define LINE_STR                EXPAND_STRINGIFY(__LINE__)
#define SOURCE_LINE_STR         __FILE__ ":" LINE_STR 

#define COUNT_PARMS2(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _, ...) _
#define COUNT_PARMS(...) COUNT_PARMS2(__VA_ARGS__, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)

#define NOOP    ((void)0)

#define tuple_n(TYPE, NAME, ...)   \
  union { \
      struct { TYPE __VA_ARGS__; }; \
      TYPE NAME[COUNT_PARMS(__VA_ARGS__)]; \
  }

#define tuple(TYPE, ...)   tuple_n(TYPE, at, __VA_ARGS__)

typedef void (*void_fp)(void);

void_fp swap_fp(void_fp *a, void_fp *b);


//    Category,       Handler
#define VITALS_CATEGORY_TABLE \
    X(First,          Vitals_NullHandler) \
    X(Error,          Vitals_NullHandler) \
    X(Precondition,   Vitals_HandleFailure) \
    X(Postcondition,  Vitals_HandleFailure) \
    X(Test,           Vitals_NullHandler) \
    X(End,            Vitals_NullHandler)

#define X(cat, ___)  Vitals_Category_##cat,
typedef enum Vitals_Category {
    VITALS_CATEGORY_TABLE 
    Vitals_Category_Size
} Vitals_Category;
#undef X

typedef enum Stat {
    Stat_OK = 0,
    Stat_Error, 
    Stat_Failure,
    Stat_TestFailed,
} Stat;

typedef struct Vitals {
    union {
        Stat status;
        _Bool is_error;
    };
    Vitals_Category category;
    const char* debug_info;
    const char* function;
    const char* message;
} Vitals;

const char *Vitals_CategoryName(Vitals_Category category);

typedef void (*Vitals_HandlerFP)(Vitals *vits, void *user_data);

Vitals_HandlerFP Vitals_SetHandler(Vitals_Category category, Vitals_HandlerFP new_handler);
Vitals_HandlerFP Vitals_SetHandlerData(Vitals_Category category, Vitals_HandlerFP new_handler, void *user_data);
Vitals_HandlerFP Vitals_GetHandler(Vitals_Category category);
void Vitals_NullHandler(Vitals *unused, void *user_data);
void Vitals_UndefinedHandler(Vitals *vit, void *user_data);
void Vitals_HandleFailure(Vitals *assertion, void *user_data);
void Vitals_TestFailure(Vitals *vits, void *user_data);

void CheckVitals(Vitals *vits);

#define MakeAssertion(CAT, MSG)  ((Vitals){ .status=(Stat_Failure), .category=(CAT), .debug_info=SOURCE_LINE_STR, .message=(MSG), .function=__func__ })
#define MakeError(STAT, MSG)     ((Vitals){ .status=(STAT), .category=(Vitals_Category_Error), .debug_info=SOURCE_LINE_STR, .function=__func__, .message=(MSG) })

#define requires(EXPR)      ((EXPR)? NOOP: CheckVitals( &MakeAssertion(Vitals_Category_Precondition, #EXPR) ))
#define ensures(EXPR)       ((EXPR)? NOOP: CheckVitals( &MakeAssertion(Vitals_Category_Postcondition, #EXPR) ))
#define test(EXPR)          ((EXPR)? NOOP: CheckVitals( &(Vitals){ .category=Vitals_Category_Test, .debug_info=SOURCE_LINE_STR, .message=STRINGIFY(EXPR), .function=__func__ } ))

