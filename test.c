#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <stddef.h>

#include "kwrlib.h"
#include "kwrmaze.h"

typedef struct Test_Runner {
    int   failure_count;
    int   test_count;
} Test_Runner;

void TestCase(Test_Runner *runner, bool passes, const char *message)
{
    ++runner->test_count;
    if (!passes) {
        printf("%s\n", message);
        ++runner->failure_count;
    }
}

#define test(EXPR)       TestCase(runner, (EXPR), SOURCE_LINE_STR ": Test \"" STRINGIFY(EXPR) "\" failed.")



typedef struct TypeInfo {
    const char * name;
} TypeInfo;

typedef const TypeInfo *TypeId;

#define TYPE_ID_NAME(type)       type##_TypeId
#define TYPE_ID_DECL(typename)   extern TypeId TYPE_ID_NAME(typename)
#define TYPE_ID_DEFN(typename)   TypeId  TYPE_ID_NAME(typename) = &(TypeInfo){ .name = #typename }

TypeId type_of(void *obj_ptr)
{
    return obj_ptr? *((TypeId*)(obj_ptr)): NULL;
}

_Bool is_type(void *object, TypeId type)
{
    return object && type_of(object) == type;
}





void TestForEachGridRow(Maze_Cell *row, void *data)
{
    int *counter = (int *)data;
    *counter += row[0].row;
}


//  enum     type         id  printf
#define BASIC_TYPES_X \
  X(Int,     int,          i, "%d") \
  X(Long,    long,         l, "%l") \
  X(Double,  double,       d, "%f") \
  X(CString, const char*,  s, "%s") \
  X(Char,    char,         c, "%c") 

#define X(ENUM_, ...)  BasicType_##ENUM_,
typedef enum {
    BASIC_TYPES_X 
    BasicType_True,
    BasicType_False,
    STANDARD_ENUM_VALUES(BasicType)
} BasicType;
#undef X

typedef struct {
    TypeInfo base_typeinfo;
    BasicType  basic_type_id;
    const char *format_spec;
} BasicTypeInfo;

typedef const BasicTypeInfo * const BasicTypeId;

// Generate TypeId for each basic type
#define X(NAME_, TYPE_, ID_, FORMAT_)  static BasicTypeInfo NAME_##_TypeId = { \
    .base_typeinfo.name = #TYPE_, \
    .basic_type_id = BasicType_##NAME_, \
    .format_spec = FORMAT_ \
};
BASIC_TYPES_X 
#undef X

// Specialized TypeId for true bools
static BasicTypeInfo True_TypeId = { \
    .base_typeinfo.name = "true",
    .basic_type_id = BasicType_True,
    .format_spec = "true"
};

// Specialized TypeId for flase bools
static BasicTypeInfo False_TypeId = { \
    .base_typeinfo.name = "false",
    .basic_type_id = BasicType_False,
    .format_spec = "false"
};


#define X(_0, TYPE, ID, _1)  TYPE ID;
typedef struct AnyVar {
    BasicTypeId typeid;
    const union { 
        BASIC_TYPES_X 
        _Bool b;
    } value;
} AnyVar;
#undef X

const char *Any_GetPrintFormat(AnyVar v)
{
    // require v.type in enum range
    return v.typeid->format_spec;
}

void Any_Print(AnyVar v)
{
    printf(Any_GetPrintFormat(v), v.value);
}

#define X(NAME_, TYPE_, MEM_, _0) \
AnyVar Any_##NAME_(TYPE_ v) { return (AnyVar){  .typeid = &NAME_##_TypeId,  .value. MEM_ = v }; }
BASIC_TYPES_X 
#undef X

AnyVar Any_Bool(_Bool v) 
{
    return v?
      (AnyVar){  .typeid = &True_TypeId,   .value.b = v }:
      (AnyVar){  .typeid = &False_TypeId,  .value.b = v };
}

#define any(V)  _Generic( (V), \
            int:          Any_Int, \
            long:         Any_Long, \
            double:       Any_Double, \
            const char*:  Any_CString, \
            char:         Any_Char, \
            _Bool:        Any_Bool \
            ) (V)

typedef struct {
    FILE *file;
    int   throttle;
} TraceConfig;

void trace(TraceConfig *tracer, const char *stamp, const char *category, int level, const char *message)
{
    // require trace not null
    if (level <= tracer->throttle) {
        fprintf(tracer->file, "%s %s %d \"%s\"\n", stamp, category, level, message);
    }
}

void debug(TraceConfig *tracer, int level, const char *sourceline, const char *message)
{
    trace(tracer, sourceline, "Debug", level, message);
}



#define error(stat_, error_code_, error_msg_)  \
  (((stat_) = (Status){ .error=(error_code_), .debug_info=SOURCE_LINE_STR, .function=__func__, .message=(error_msg_) }).error)


#define TIMESTAMP_STR_LEN 32
typedef struct { char str[TIMESTAMP_STR_LEN]; } TimeStamp;

ErrorCode GetTimeStamp(TimeStamp *stamp, Status *stat)
{
    requires(stamp != NULL);

    time_t now = time(NULL);
    if (now == (time_t)(-1)) {
        return error(*stat, ErrorCode_Failure, "GetCurrentTime() returned -1");
    }

    struct tm *local_tm = localtime(&now);
    if (!local_tm) {
        return error(*stat, ErrorCode_Failure, "localtime() returned NULL");
    }

    size_t strftime_returns = strftime(stamp->str, TIMESTAMP_STR_LEN, "%Y-%m-%d %H:%M:%S", local_tm);

    ensure(strftime_returns > 0);
    return ErrorCode_OK;
}

void print_log(TraceConfig *tracer, int level, const char *message)
{ 
    Status stat;
    TimeStamp timestamp = {0};
    if (GetTimeStamp(&timestamp, &stat) != ErrorCode_OK) {
        trace(tracer, stat.debug_info, ErrorCode_String(stat.error), 1, stat.message);
    }
    trace(tracer, timestamp.str, "Debug", level, message);
}





void RunTests(Test_Runner *runner)
{
    UNUSED(runner);

    test(true);
    //test(false);
    
    { // TypeId tests

        TYPE_ID_DEFN(TypedFoo);

        typedef struct TypedFoo {
            TypeId type_info;
            int x;
        } TypedFoo;

        typedef struct UntypedFoo {
            float y;
        } UntypedFoo;

        TypedFoo   *nfoo = NULL;
        TypedFoo   tfoo = { .type_info = TypedFoo_TypeId, .x = 1 };
        UntypedFoo ufoo = { .y = 0.5 };

        test( !type_of(nfoo) );
        test( type_of(&tfoo) == TypedFoo_TypeId );
        test( type_of(&tfoo) != type_of(&ufoo) );
    }

    { // tuple macro
        vector(int, x, y, z) point = {{ 10, 11, 12 }};
        test(vec_length(point) == 3);
        test(point.x == 10);
        test(point.y == 11);
        test(point.z == 12);
        test(point.components[0] == 10);
        test(point.components[1] == 11);
        test(point.components[2] == 12);
    }

    { // dynarray 

        test(sizeof(dynarray(int)) == sizeof(dynarray(char)));
        test(offsetof(dynarray(int), top) == offsetof(dynarray(char), top));
        test(offsetof(dynarray(int), end) == offsetof(dynarray(char), end));
        test(offsetof(dynarray(int), begin) == offsetof(dynarray(char), begin));

        dynarray(int) *a = new_dynarray(int); 

        test(capacity(a) == 64);
        test(length(a) == 0);
        test(remaining(a) == 64);
        test(is_empty(a));
        test(!is_full(a));

        int x = 101;
        int i = push(a, x++); // = x++;

        // these don't compile because the macros are not L-values
        //capacity(a) = 5;
        //length(a) = 3;
        //remaining(a) = 99;
        //da_set(a, 0, 1000) = 0;

        test(capacity(a) == 64);
        test(length(a) == 1);
        test(remaining(a) == 63);
        test(!is_empty(a));
        test(!is_full(a));
        test(da_get(a, 0) == 101);
        test(i == 101);

        i = da_set(a, 0, 99);
        test(a->begin[0] == 99);
        test(i == 99);

        for (int i = 47; i --> 0;)  push(a, x++); // = x++;

        test(length(a) == 48);
        test(remaining(a) == 16);
        test(!is_empty(a));
        test(!is_full(a));
        test(da_get(a, 1) == 102);
        test(da_get(a, 47) == 148);

        int *ab = a->begin;
        test(ab[2] == 103);
        test(ab[46] == 147);

        while (remaining(a))  push(a, x++); // = x++;

        test(length(a) == 64);
        test(remaining(a) == 0);
        test(!is_empty(a));
        test(is_full(a));
        ab = a->begin;
        test(ab[63] == 164);

        void *b = enlarge(a);
        test(b != NULL);
        a = b;

        test(capacity(a) == 128);
        test(length(a) == 64);
        test(remaining(a) == 64);
        test(!is_empty(a));
        test(!is_full(a));
        
        test(da_get(a, 1) == 102);
        test(da_get(a, 40) == 141);
        test(da_get(a, 63) == 164);

        // Should assert
        //int y = da_get(a, 129);
        
        a = enlarge(a, 44);
        test(capacity(a) == 128+44);

        free(a);
    }

    { // Trace

        //TraceConfig tracer = { .file = stdout, .throttle = 8 };
        //debug(&tracer, 5, SOURCE_LINE_STR, "Hello, world!");
        //debug(&tracer, 10, SOURCE_LINE_STR, "This should not print.");

        //TraceConfig logger = { 
            //.file = fopen("trace_test.log", "w"),
            //.throttle = 8 
        //};

        //print_log(&logger, 3, "Write this to log file");

        //Status stat = { 
            //.error = ErrorCode_Error,
            //.debug_info = SOURCE_LINE_STR,
            //.function = __func__,
            //.message = "Write to log"
        //};

        //fclose(logger.file);
    }

    { // Maze Cells
        Maze_Cell cell_1 = { .row = 1, .column = 3 };
        test(cell_1.row == 1);
        test(cell_1.column == 3);
        test(cell_1.north == NULL);
        test(cell_1.south == NULL);
        test(cell_1.east == NULL);
        test(cell_1.west == NULL);

        Maze_Cell cell_n = { .row = 0, .column = 3 };
        Maze_LinkCells(&cell_1, Maze_Dir_North, &cell_n);
        test(cell_1.north == &cell_n);
        test(cell_n.south == &cell_1);

        Maze_Cell cell_e = { .row = 1, .column = 4 };
        Maze_LinkCells(&cell_1, Maze_Dir_East, &cell_e);
        test(cell_1.east = &cell_e);
        test(cell_e.west = &cell_1);

        Maze_Cell cell_s = { .row = 2, .column = 3, .north = &cell_1 };
        cell_1.south = &cell_s;
        test(Maze_FindLink(&cell_1, &cell_s) == &cell_1.south);
        test(Maze_FindLink(&cell_s, &cell_1) == &cell_s.north);
        Maze_UnlinkCells(&cell_1, &cell_s);
        test(cell_s.north == NULL);
        test(cell_1.south == NULL);

        Maze_Cell cell_0;
        test(!Maze_FindLink(&cell_1, &cell_0));
    }

    { // Maze Grid
        Maze_Grid grid = { .num_rows = 10, .num_columns = 20 };
        test(grid.num_rows == 10);
        test(grid.num_columns == 20);
        test(!grid.cells);
        test(!grid.rows);

        Maze_InitGrid(&grid);
        test(grid.cells != NULL);
        test(grid.rows != NULL);
        for (int r = 0; r < grid.num_rows; ++r) {
            for (int c = 0; c < grid.num_columns; ++c) {
                test(grid.rows[r][c].row == r);
                test(grid.rows[r][c].column == c);
            }
        }

        test(Maze_GridCellAt(&grid,  0,  0));
        test(Maze_GridCellAt(&grid,  5,  5));
        test(Maze_GridCellAt(&grid,  9, 19));
        test(Maze_GridCellAt(&grid, -1,  0) == NULL);
        test(Maze_GridCellAt(&grid,  0, -1) == NULL);
        test(Maze_GridCellAt(&grid, 10, 19) == NULL);
        test(Maze_GridCellAt(&grid,  9, 20) == NULL);

        test(Maze_CountGridCells(&grid) == 200);

        Maze_Cell *cell = Maze_GridCellAt(&grid, 5,5);
        Maze_Cell *north_cell = Maze_GoNorth(&grid, cell);
        test(north_cell->row == 4 && north_cell->column == 5);

        Maze_Cell *east_cell = Maze_GoEast(&grid, cell);
        test(east_cell->row == 5 && east_cell->column == 6);

        Maze_Cell *top_right_cell = Maze_GridCellAt(&grid, 0, 19);
        test(NULL == Maze_GoNorth(&grid, top_right_cell));
        test(NULL == Maze_GoEast(&grid, top_right_cell));

        int row_count = 0;
        Maze_ForEachGridRow(&grid, TestForEachGridRow, &row_count);
        test(row_count == 45); // 0 + 1 + ... + 9

        Maze_DisposeGrid(&grid);
        test(grid.num_rows == 0);
        test(grid.num_columns == 0);
        test(!grid.cells);
        test(!grid.rows);
    }

}

Test_Runner Test_MakeRunner()
{
    return (Test_Runner){ .failure_count = 0, .test_count = 0 };
}

int main(int argc, char *argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    printf("Testing...\n");

    Test_Runner runner = Test_MakeRunner(); 

    RunTests(&runner);

    if (runner.failure_count) {
        printf("UH OH!  %d test failure%c\n", runner.failure_count, (char[]){' ','s'}[runner.failure_count > 1]);
    }
    else {
        printf("All good!  %d tests passed.\n", runner.test_count);
    }

    return runner.failure_count;
}

