#define KWRLIB_H_INCLUDED

#define STRINGIFY(x)            #x
#define EXPAND_STRINGIFY(x)     STRINGIFY(x)

#define LINE_STR                EXPAND_STRINGIFY(__LINE__)
#define SOURCE_LINE_STR         __FILE__ ":" LINE_STR 

#define COUNT_PARMS2(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _, ...) _
#define COUNT_PARMS(...) COUNT_PARMS2(__VA_ARGS__, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)
	
#define tuple_n(TYPE, NAME, ...)   \
  union { \
      struct { TYPE __VA_ARGS__; }; \
      TYPE NAME[COUNT_PARMS(__VA_ARGS__)]; \
  }

#define tuple(TYPE, ...)   tuple_n(TYPE, at, __VA_ARGS__)

typedef enum Stat {
    Stat_OK = 0,
    Stat_ERROR, 
    Stat_INIT_FAILED,    // Initialer function failed
    Stat_TestFailed,
} Stat;

//----------------------------------------------------------------------
// Test: unit testing module

typedef struct Test_Runner Test_Runner;
void RunTests(Test_Runner *runner);
void Test_Assert_f(Test_Runner *runner, bool condition, const char *message);

#define Test_Assert(CONDITION)   \
do{ Test_Assert_f(runner, (CONDITION), SOURCE_LINE_STR ": Test failed: " STRINGIFY(CONDITION)); }while(0)


