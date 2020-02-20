#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include "kwrlib.h"
#include "kwrmaze.h"

typedef struct TypeInfo {
    const char *name;
} TypeInfo;

typedef struct Base {
     const TypeInfo *type_info;
} Base;

#define typeid(obj)    (((Base*)obj)->type_info)

_Bool is_type(void *object, const TypeInfo *type)
{
    return object && typeid(object) == type;
}



const TypeInfo Test_Runner_TypeId = { .name = "Test_Runner" };

typedef struct Test_Runner {
    Base  base;
    int   failure_count;
    int   test_count;
} Test_Runner;

void Vitals_TestFailure(Vitals *vits, void *user_data)
{
    printf("%s: %s \"%s\" failed in function %s()\n", vits->debug_info, Vitals_CategoryName(vits->category), vits->message, vits->function);
    if (is_type(user_data, &Test_Runner_TypeId)) {
        Test_Runner *runner = user_data;
        ++runner->failure_count;
    }
}

#define ARRAY(TYPE)  struct { size_t size; TYPE data[]; }


void TestForEachGridRow(Maze_Cell *row, void *data)
{
    int *counter = (int *)data;
    *counter += row[0].row;
}


// Mocks for testing failed assertions
static char TEST_VITALS_MOCK_STR[200] = "";

void Vitals_MockHandler(Vitals *assertion, void *user_data)
{
    sprintf(TEST_VITALS_MOCK_STR, "%s: ASSERT: %s \"%s\" failed in function %s()\n", assertion->debug_info, Vitals_CategoryName(assertion->category), assertion->message, assertion->function);
}

void RunTests(Test_Runner *runner)
{
    test(true);
    //test(false);
    
    // tuple macro
    {
        tuple(int, x, y, z) point = { 10, 11, 12 };
        test(point.x == 10);
        test(point.y == 11);
        test(point.z == 12);
        test(point.at[0] == 10);
        test(point.at[1] == 11);
        test(point.at[2] == 12);
    }

    // ARRAY macro
    {
        int size = 100;
        ARRAY(int) *a = malloc( sizeof(ARRAY(int)) + (sizeof(int) * size) );
        a->size = 100;
        a->data[10] = 100;
        test(a->size == 100);
        test(a->data[10] == 100);
        free(a);
    }

    // Maze Cells
    {
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

    { // Precondition assertion Tests
        // Uncomment to test the real handler
        //requires(false);

        Vitals_HandlerFP last_pre_handler = Vitals_SetHandler(Vitals_Category_Precondition, Vitals_MockHandler);
        Vitals_HandlerFP last_post_handler = Vitals_SetHandler(Vitals_Category_Postcondition, Vitals_MockHandler);

        test(last_pre_handler == Vitals_HandleFailure);
        test(last_post_handler == Vitals_HandleFailure);

        int x = 0;

        TEST_VITALS_MOCK_STR[0] = '\0';
        requires(x == 0); 
        test(!strlen(TEST_VITALS_MOCK_STR));

        // On the same line so LINE_STR picks up the correct line number.
        requires(x == 1);  test(!strcmp(TEST_VITALS_MOCK_STR, "test.c:" LINE_STR ": ASSERT: Precondition \"x == 1\" failed in function RunTests()\n"));
        ensures(x == 2);   test(!strcmp(TEST_VITALS_MOCK_STR, "test.c:" LINE_STR ": ASSERT: Postcondition \"x == 2\" failed in function RunTests()\n"));

        Vitals_SetHandler(Vitals_Category_Precondition, last_pre_handler);
        Vitals_SetHandler(Vitals_Category_Postcondition, last_post_handler);
    }
}

int main(int argc, char *argv[])
{
    printf("Testing...\n");

    Test_Runner runner = { .base.type_info = &Test_Runner_TypeId, .failure_count = 0, .test_count = 0 };

    Vitals_SetHandlerData(Vitals_Category_Test, Vitals_TestFailure, &runner);

    RunTests(&runner);

    if (runner.failure_count) {
        printf("UH OH!  %d test failure%c\n", runner.failure_count, (char[]){' ','s'}[runner.failure_count > 1]);
    }
    else {
        printf("All good!  %d tests passed.\n", runner.test_count);
    }

    return runner.failure_count;
}

