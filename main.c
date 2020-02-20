#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <SDL.h>

#include "kwrlib.h"
#include "kwrmaze.h"

#define array_length(a)  sizeof(a)/sizeof(a[0])

void PrintError(Vitals error)
{
    printf("%s: Error in %s(), \"%s\"\n", error.debug_info, error.function, error.message);
}

void Vitals_Print(Vitals *vits)
{
    printf("%s: %s \"%s\" in function %s()\n", vits->debug_info, Vitals_CategoryName(vits->category), vits->message, vits->function);
}

typedef struct Game_Driver {
    _Bool running;
    SDL_Window* window;
    SDL_Renderer* renderer;
} Game_Driver;

// If any SDL calls fail, 
// return an error indicator that can be checked easily
// report debug info, function name, SDL error & string
Stat Game_Init(Game_Driver *driver, Vitals *error)
{
    requires(driver != NULL);

    *driver = (Game_Driver){ 0 };

    int sdl_code = SDL_Init(SDL_INIT_VIDEO);
    if (sdl_code) return (*error = MakeError(Stat_Error, SDL_GetError())).status;

    driver->window   = SDL_CreateWindow("Hello World", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 800, SDL_WINDOW_SHOWN);
    if (!driver->window) return (*error = MakeError(Stat_Error, SDL_GetError())).status;

    driver->renderer = SDL_CreateRenderer(driver->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!driver->renderer) return (*error = MakeError(Stat_Error, SDL_GetError())).status;

    return Stat_OK;
}

void Game_Dispose(Game_Driver *driver)
{
    if (driver->renderer)  SDL_DestroyRenderer(driver->renderer);
    if (driver->window)    SDL_DestroyWindow(driver->window);
    SDL_Quit();
}


void Maze_BinaryTree(Maze_Grid *grid)
{
    srand(23);
    Maze_Cell *cell = grid->cells; 
    int count = Maze_CountGridCells(grid);
    while (count--) {

        struct Cell_Neighbor { Maze_Dir dir; Maze_Cell *cell; } neighbors[2];
        int num = 0;

        Maze_Cell *neighbor;
        if ( (neighbor = Maze_GoNorth(grid, cell)) ) {
            neighbors[num++] = (struct Cell_Neighbor){ Maze_Dir_North, neighbor };
        }

        if ( (neighbor = Maze_GoEast(grid, cell)) ) {
            neighbors[num++] = (struct Cell_Neighbor){ Maze_Dir_East, neighbor };
        }

        if (num) {
            int choose = rand() % num;
            Maze_LinkCells(cell, neighbors[choose].dir, neighbors[choose].cell);
        }

        ++cell;
    }
}

int main(int argc, char* argv[])
{
    Vitals err = { Stat_OK };
    Game_Driver driver;

    if (Stat_OK != Game_Init(&driver, &err)) {
        Vitals_Print(&MakeError(Stat_Error, "Failed to initialize game driver"));
        Vitals_Print(&err);
    }
    else {
        Maze_Grid grid = { .num_rows = 10, .num_columns = 10 };
        Maze_InitGrid(&grid);
        Maze_BinaryTree(&grid);

        //setup();
        for (driver.running = true; driver.running; ) {

            for (SDL_Event event; SDL_PollEvent(&event) != 0; ) {
                if (event.type == SDL_QUIT) {
                    driver.running = false;
                }
            }

            // update game state

            // clear back buffer
            SDL_SetRenderDrawColor(driver.renderer, 0, 0, 0, 255);
            SDL_RenderClear(driver.renderer);

            // render & display frame

            // check return code
            SDL_SetRenderDrawColor(driver.renderer, 255, 255, 255, 255);

            // check return code
            for (int r = 0; r < grid.num_rows; ++r) {
                for (int c = 0; c < grid.num_columns; ++c) {
                    int x1 = (grid.rows[r][c].column + 1)  * 50;
                    int y1 = (grid.rows[r][c].row    + 1)  * 50;
                    int x2 = x1 + 50;
                    int y2 = y1 + 50;

                    if (!grid.rows[r][c].north) {
                        SDL_RenderDrawLine(driver.renderer, x1, y1, x2, y1);
                    }

                    if (!grid.rows[r][c].west) {
                        SDL_RenderDrawLine(driver.renderer, x1, y1, x1, y2);
                    }

                    if (grid.rows[r][c].column == grid.num_columns-1) {
                        SDL_RenderDrawLine(driver.renderer, x2, y1, x2, y2);
                    }

                    if (grid.rows[r][c].row == grid.num_rows-1) {
                        SDL_RenderDrawLine(driver.renderer, x1, y2, x2, y2);
                    }

                }
            }

            SDL_RenderPresent(driver.renderer);
        }

        Maze_DisposeGrid(&grid);
    }

    Game_Dispose(&driver);

    return err.status;
}

