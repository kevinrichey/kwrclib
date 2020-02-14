// Requires kwrlib.h

typedef enum {
    Maze_Dir_First,
    Maze_Dir_North = Maze_Dir_First, 
    Maze_Dir_East,
    Maze_Dir_West,
    Maze_Dir_South,
    Maze_Dir_Last = Maze_Dir_South,
    Maze_Dir_End
} Maze_Dir;

typedef struct Maze_Cell {
    int row, column;
    tuple_n(struct Maze_Cell, *links, *north, *east, *west, *south);
} Maze_Cell;

typedef struct Maze_Grid {
    int num_rows, num_columns;
    Maze_Cell *cells;
    Maze_Cell **rows;
} Maze_Grid;

typedef void (*Maze_GridRowFn)(Maze_Cell *row, void *data);

Maze_Cell  **Maze_FindLink       (Maze_Cell *cell, Maze_Cell *find_it);
void         Maze_LinkCells      (Maze_Cell *cell, Maze_Dir dir, Maze_Cell *to_cell);
void         Maze_UnlinkCells    (Maze_Cell *cell, Maze_Cell *cell_b);
void         Maze_InitGrid       (Maze_Grid *grid);
Maze_Cell   *Maze_GridCellAt     (Maze_Grid *grid, int row, int col);
int          Maze_CountGridCells (Maze_Grid *grid);
void         Maze_ForEachGridRow (Maze_Grid *grid, Maze_GridRowFn row_op, void *pass);
void         Maze_DisposeGrid    (Maze_Grid *grid);
Maze_Cell   *Maze_GoNorth        (Maze_Grid *grid, Maze_Cell *cell);
Maze_Cell   *Maze_GoEast         (Maze_Grid *grid, Maze_Cell *cell);

