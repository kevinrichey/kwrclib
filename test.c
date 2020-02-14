#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
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

}

int main(int argc, char *argv[])
{
    printf("Testing... ");

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

