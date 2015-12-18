#define _GNU_SOURCE	// qsort_r

#include <ncurses.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define CELL_HEIGHT 4
#define CELL_WIDTH 7
#define N_RANDOM_BRICKS 3
#define LINE_LEN 5

typedef struct _Cell {
	int y;
	int x;
} Cell;

typedef struct _Game {
	Cell cur_cell;
	Cell cur_brick;
	int score;
	unsigned int board[9][9];
	// TODO: N_TURNS
	/* int N_FREE_CELLS = 9 * 9; */
} Game;

void draw_cell_border(int y, int x, short pair);
void deselect_cell(int y, int x);
void select_cell(Game *G, int y, int x);
void select_brick(Game *G, int y, int x);
void fill_cell(int y, int x, short pair);
void put_brick(Game *G, int y, int x, short color);
Cell** get_adj_cells(Game *G, int y, int x, int* adj_cells_n);
int dist(int from_y, int from_x, int to_y, int to_x);
int compare_cells(const void *a, const void *b, void *G);
void free_adj_cells(Cell **A, int n);
bool path_exists(Game *G, int from_y, int from_x, int to_y, int to_x, bool visited[9][9], int seqn);
void remove_brick(Game *G, int y, int x, bool draw_border);
bool move_selected_brick(Game *G, int y, int x);
void score(Game *G, int n);
void reduce_bricks(Game *G, int at_y, int at_x);
int randint(int n);
void put_bricks_random(Game *G, int n);
int get_n_free_cells(Game *G);
void die(int status);
