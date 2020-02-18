#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include "kwrlib.h"
#include "kwrmaze.h"

typedef struct Test_Runner {
    int failure_count;
    int test_count;
} Test_Runner;

void Test_Assert_f(Test_Runner *runner, bool condition, const char *message)
{
    ++runner->test_count;
    if (!condition) {
        printf("%s\n", message);
        ++runner->failure_count;
    }
}


#define ARRAY(TYPE)  struct { size_t size; TYPE data[]; }


void TestForEachGridRow(Maze_Cell *row, void *data)
{
    int *counter = (int *)data;
    *counter += row[0].row;
}


#define NOOP    ((void)0)

#define X(cat)  [Assert_Category_##cat] = #cat,
static const char *kwr_ASSERT_CATEGORY_NAMES[Assert_Category_Size] = {
    KWR_ASSERT_CATEGORY_TABLE 
};
#undef X

typedef void (*Assert_HandlerFP)(Error *assertion);

void Assert_HandleFailure(Error *assertion)
{
    printf("%s: ASSERT: %s \"%s\" failed in function %s()\n", assertion->debug_info, kwr_ASSERT_CATEGORY_NAMES[assertion->category], assertion->message, assertion->function);
    abort();
}

static Assert_HandlerFP handler[Assert_Category_Size] = { 
    Assert_HandleFailure,
    Assert_HandleFailure,
    Assert_HandleFailure,
};

typedef void (*void_fp)(void);

void_fp swap_fp(void_fp *a, void_fp *b)
{
    void_fp t = *a;
    *a = *b;
    *b = t;
    return *a;
}

Assert_HandlerFP Assert_SetHandler(Assert_Category category, Assert_HandlerFP new_handler)
{
    return (0 < category && category < Assert_Category_End)?
      (Assert_HandlerFP)swap_fp((void_fp*)&new_handler, (void_fp*)&handler[category]):
      NULL;
}

Assert_HandlerFP Assert_GetHandler(Assert_Category category)
{
    return (0 < category && category < Assert_Category_End)?  handler[category]:  NULL;
}

void kwr_AssertFailed(Error *assertion)
{
    Assert_HandlerFP handler = Assert_GetHandler(assertion->category);
    if (handler) {
        handler(assertion);
    }
}

#define requires(EXPR)      ((EXPR)? NOOP: kwr_AssertFailed( &MakeAssertion(Assert_Category_Precondition, #EXPR)
#define ensures(EXPR)       ((EXPR)? NOOP: kwr_AssertFailed( &MakeAssertion(Assert_Category_Postcondition, #EXPR)


// Mocks for testing failed assertions
static char TEST_ASSERT_MOCK_STR[200] = "";

void Assert_MockHandler(Error *assertion)
{
    sprintf(TEST_ASSERT_MOCK_STR, "%s: ASSERT: %s \"%s\" failed in function %s()\n", assertion->debug_info, kwr_ASSERT_CATEGORY_NAMES[assertion->category], assertion->message, assertion->function);
}

void RunTests(Test_Runner *runner)
{
    Test_Assert(true);
    //Test_Assert(false);
    
    // tuple macro
    {
        tuple(int, x, y, z) point = { 10, 11, 12 };
        Test_Assert(point.x == 10);
        Test_Assert(point.y == 11);
        Test_Assert(point.z == 12);
        Test_Assert(point.at[0] == 10);
        Test_Assert(point.at[1] == 11);
        Test_Assert(point.at[2] == 12);
    }

    // ARRAY macro
    {
        int size = 100;
        ARRAY(int) *a = malloc( sizeof(ARRAY(int)) + (sizeof(int) * size) );
        a->size = 100;
        a->data[10] = 100;
        Test_Assert(a->size == 100);
        Test_Assert(a->data[10] == 100);
        free(a);
    }

    // Maze Cells
    {
        Maze_Cell cell_1 = { .row = 1, .column = 3 };
        Test_Assert(cell_1.row == 1);
        Test_Assert(cell_1.column == 3);
        Test_Assert(cell_1.north == NULL);
        Test_Assert(cell_1.south == NULL);
        Test_Assert(cell_1.east == NULL);
        Test_Assert(cell_1.west == NULL);

        Maze_Cell cell_n = { .row = 0, .column = 3 };
        Maze_LinkCells(&cell_1, Maze_Dir_North, &cell_n);
        Test_Assert(cell_1.north == &cell_n);
        Test_Assert(cell_n.south == &cell_1);

        Maze_Cell cell_e = { .row = 1, .column = 4 };
        Maze_LinkCells(&cell_1, Maze_Dir_East, &cell_e);
        Test_Assert(cell_1.east = &cell_e);
        Test_Assert(cell_e.west = &cell_1);

        Maze_Cell cell_s = { .row = 2, .column = 3, .north = &cell_1 };
        cell_1.south = &cell_s;
        Test_Assert(Maze_FindLink(&cell_1, &cell_s) == &cell_1.south);
        Test_Assert(Maze_FindLink(&cell_s, &cell_1) == &cell_s.north);
        Maze_UnlinkCells(&cell_1, &cell_s);
        Test_Assert(cell_s.north == NULL);
        Test_Assert(cell_1.south == NULL);

        Maze_Cell cell_0;
        Test_Assert(!Maze_FindLink(&cell_1, &cell_0));
    }

    { // Maze Grid
        Maze_Grid grid = { .num_rows = 10, .num_columns = 20 };
        Test_Assert(grid.num_rows == 10);
        Test_Assert(grid.num_columns == 20);
        Test_Assert(!grid.cells);
        Test_Assert(!grid.rows);

        Maze_InitGrid(&grid);
        Test_Assert(grid.cells != NULL);
        Test_Assert(grid.rows != NULL);
        for (int r = 0; r < grid.num_rows; ++r) {
            for (int c = 0; c < grid.num_columns; ++c) {
                Test_Assert(grid.rows[r][c].row == r);
                Test_Assert(grid.rows[r][c].column == c);
            }
        }

        Test_Assert(Maze_GridCellAt(&grid,  0,  0));
        Test_Assert(Maze_GridCellAt(&grid,  5,  5));
        Test_Assert(Maze_GridCellAt(&grid,  9, 19));
        Test_Assert(Maze_GridCellAt(&grid, -1,  0) == NULL);
        Test_Assert(Maze_GridCellAt(&grid,  0, -1) == NULL);
        Test_Assert(Maze_GridCellAt(&grid, 10, 19) == NULL);
        Test_Assert(Maze_GridCellAt(&grid,  9, 20) == NULL);

        Test_Assert(Maze_CountGridCells(&grid) == 200);

        Maze_Cell *cell = Maze_GridCellAt(&grid, 5,5);
        Maze_Cell *north_cell = Maze_GoNorth(&grid, cell);
        Test_Assert(north_cell->row == 4 && north_cell->column == 5);

        Maze_Cell *east_cell = Maze_GoEast(&grid, cell);
        Test_Assert(east_cell->row == 5 && east_cell->column == 6);

        Maze_Cell *top_right_cell = Maze_GridCellAt(&grid, 0, 19);
        Test_Assert(NULL == Maze_GoNorth(&grid, top_right_cell));
        Test_Assert(NULL == Maze_GoEast(&grid, top_right_cell));

        int row_count = 0;
        Maze_ForEachGridRow(&grid, TestForEachGridRow, &row_count);
        Test_Assert(row_count == 45); // 0 + 1 + ... + 9

        Maze_DisposeGrid(&grid);
        Test_Assert(grid.num_rows == 0);
        Test_Assert(grid.num_columns == 0);
        Test_Assert(!grid.cells);
        Test_Assert(!grid.rows);
    }

    { // Precondition assertion Tests
        // Uncomment to test the real handler
        //requires(false);

        Assert_HandlerFP last_pre_handler = Assert_SetHandler(Assert_Category_Precondition, Assert_MockHandler);
        Assert_HandlerFP last_post_handler = Assert_SetHandler(Assert_Category_Postcondition, Assert_MockHandler);

        Test_Assert(last_pre_handler == Assert_HandleFailure);
        Test_Assert(last_post_handler == Assert_HandleFailure);

        int x = 0;

        TEST_ASSERT_MOCK_STR[0] = '\0';
        requires(x == 0); 
        Test_Assert(!strlen(TEST_ASSERT_MOCK_STR));

        // On the same line so LINE_STR picks up the correct line number.
        requires(x == 1);  Test_Assert(!strcmp(TEST_ASSERT_MOCK_STR, "test.c:" LINE_STR ": ASSERT: Precondition \"x == 1\" failed in function RunTests()\n"));
        ensures(x == 2);   Test_Assert(!strcmp(TEST_ASSERT_MOCK_STR, "test.c:" LINE_STR ": ASSERT: Postcondition \"x == 2\" failed in function RunTests()\n"));

        Assert_SetHandler(Assert_Category_Precondition, last_pre_handler);
        Assert_SetHandler(Assert_Category_Postcondition, last_post_handler);
    }
}

int main(int argc, char *argv[])
{
    printf("Testing...\n");

    Test_Runner runner = { .failure_count = 0, .test_count = 0 };

    RunTests(&runner);

    if (runner.failure_count) {
        printf("UH OH!  %d test failure%c\n", runner.failure_count, (char[]){' ','s'}[runner.failure_count > 1]);
    }
    else {
        printf("All good!  %d tests passed.\n", runner.test_count);
    }

    return runner.failure_count;
}

