#include <stdbool.h>
#include <stdlib.h>

#include "kwrlib.h"
#include "kwrmaze.h"

void Maze_LinkCells(Maze_Cell *from_cell, Maze_Dir dir, Maze_Cell *to_cell)
{
    requires(from_cell);
    requires(to_cell);
    requires(Maze_Dir_First <= dir && dir < Maze_Dir_End);

    Maze_Dir opposite_dir = Maze_Dir_Last - dir;
    from_cell->links[dir] = to_cell;
    to_cell->links[opposite_dir] = from_cell;
}

Maze_Cell **Maze_FindLink(Maze_Cell *cell, Maze_Cell *find_it)
{
    requires(cell);
    requires(find_it);

    Maze_Cell **links = cell->links;
    for (Maze_Cell **end = &links[Maze_Dir_End]; links != end; ++links) {
        if (*links == find_it) return links;
    }
    return NULL;
}

void Maze_UnlinkCells(Maze_Cell *cell, Maze_Cell *cell_b)
{
    requires(cell);
    requires(cell_b);

    Maze_Cell **link;
    if ((link = Maze_FindLink(cell, cell_b)))  *link = NULL;
    if ((link = Maze_FindLink(cell_b, cell)))  *link = NULL;
}

void Maze_InitGrid(Maze_Grid *grid)
{
    requires(grid);

    int num_cells = grid->num_rows * grid->num_columns;
    grid->cells   = malloc(num_cells * sizeof(Maze_Cell));
    grid->rows    = malloc(grid->num_rows * sizeof(Maze_Cell*));

    Maze_Cell *cell = grid->cells;
    for (int r = 0; r < grid->num_rows; ++r) {
        grid->rows[r] = cell;
        for (int c = 0; c < grid->num_columns; ++c) {
            *cell++ = (Maze_Cell){ .row = r, .column = c };
        }
    }

}

Maze_Cell *Maze_GridCellAt(Maze_Grid *grid, int row, int col)
{
    requires(grid);
    
    if (row < 0 || row >= grid->num_rows)     return NULL;
    if (col < 0 || col >= grid->num_columns)  return NULL;
    return &grid->rows[row][col];
}

int Maze_CountGridCells(Maze_Grid *grid)
{
    requires(grid);
    return grid->num_rows * grid->num_columns;
}

void Maze_ForEachGridRow(Maze_Grid *grid, Maze_GridRowFn row_op, void *pass)
{
    requires(grid);
    requires(row_op);

    for (int r = 0; r < grid->num_rows; ++r) {
        row_op(grid->rows[r], pass);
    }
}

void Maze_DisposeGrid(Maze_Grid *grid)
{
    if (grid) {  // okay to pass NULL, just ignore it
        if (grid->cells)   free(grid->cells);
        if (grid->rows)     free(grid->rows);
        *grid = (Maze_Grid){0};
    }
}

Maze_Cell *Maze_GoNorth(Maze_Grid *grid, Maze_Cell *cell)
{
    requires(grid);
    requires(cell);
    return (cell->row > 0)? &grid->rows[cell->row-1][cell->column]: NULL;
}

Maze_Cell *Maze_GoEast(Maze_Grid *grid, Maze_Cell *cell)
{
    requires(grid);
    requires(cell);
    return (cell->column < grid->num_columns-1)? &grid->rows[cell->row][cell->column+1]: NULL;
}

