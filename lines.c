#include "lines.h"

extern WINDOW *DEBUG_WIN, *BOARD_WIN, *INFO_WIN, *HELP_WIN;

void draw_cell_border(int y, int x, short pair)
{
	wattron(BOARD_WIN, pair);
	y *= (CELL_HEIGHT - 1);
	x *= (CELL_WIDTH - 1);
	mvwaddch(BOARD_WIN, y, x, ACS_ULCORNER);
	mvwhline(BOARD_WIN, y, x + 1, ACS_HLINE, CELL_WIDTH - 2);
	mvwaddch(BOARD_WIN, y, x + CELL_WIDTH - 1, ACS_URCORNER);
	for (int i = 1; i < CELL_HEIGHT-1; i++) {
		mvwaddch(BOARD_WIN, y + i, x, ACS_VLINE);
		mvwaddch(BOARD_WIN, y + i, x + CELL_WIDTH - 1, ACS_VLINE);
	}
	mvwaddch(BOARD_WIN, y + CELL_HEIGHT - 1, x, ACS_LLCORNER);
	mvwhline(BOARD_WIN, y + CELL_HEIGHT - 1, x + 1,
			ACS_HLINE, CELL_WIDTH - 2);
	mvwaddch(BOARD_WIN, y + CELL_HEIGHT - 1, x + CELL_WIDTH - 1,
			ACS_LRCORNER);
	wattroff(BOARD_WIN, pair);
	wrefresh(BOARD_WIN);
}

void deselect_cell(int y, int x)
{
	draw_cell_border(y, x, COLOR_PAIR(1));
	box(BOARD_WIN, 0, 0);
}

void select_cell(Game *G, int y, int x)
{
	if (y < 0 || x < 0 || y > 8 || x > 8) {
		wrefresh(BOARD_WIN);
		return;
	}
	deselect_cell(G->cur_cell.y, G->cur_cell.x);
	if (G->cur_brick.y != -1 && G->cur_brick.x != -1) {
		draw_cell_border(G->cur_brick.y, G->cur_brick.x, COLOR_PAIR(3));
	}
	G->cur_cell.y = y;
	G->cur_cell.x = x;
	draw_cell_border(y, x, COLOR_PAIR(2));
}

// TODO: make it blink?
void select_brick(Game *G, int y, int x)
{
	if (G->cur_brick.y != -1 && G->cur_brick.x != -1) {
		deselect_cell(G->cur_brick.y, G->cur_brick.x);
	}
	draw_cell_border(y, x, COLOR_PAIR(3));
	G->cur_brick.y = y;
	G->cur_brick.x = x;
}

void fill_cell(int y, int x, short pair)
{
	wattron(BOARD_WIN, pair);
	y *= (CELL_HEIGHT - 1);
	x *= (CELL_WIDTH - 1);
	for (int j = 2; j < CELL_WIDTH-1; j++) {
		for (int i = 1; i < CELL_HEIGHT-1; i++) {
			mvwaddch(BOARD_WIN, y + i, x + j, ' ');
		}
	}
	wattroff(BOARD_WIN, pair);
	wrefresh(BOARD_WIN);
}

void put_brick(Game *G, int y, int x, short color)
{
	G->board[y][x] = color;
	fill_cell(y, x, COLOR_PAIR(10 + color));
}

/* void put_symbol(int y, int x, char symb) */
/* { */
	/* y *= CELL_HEIGHT - 1; */
	/* y += CELL_HEIGHT / 2; */
	/* x *= CELL_WIDTH - 1; */
	/* x += CELL_WIDTH / 2; */
	/* mvwaddch(BOARD_WIN, y, x, symb); */
	/* wrefresh(BOARD_WIN); */
/* } */

// FIXME: this function is probably redundant and its body should be moved to
// path_exists()
Cell** get_adj_cells(Game *G, int y, int x, int* adj_cells_n)
{
	*adj_cells_n = 0;
	int adj_y, adj_x;
	/* Cell *A[9]; */
	Cell *A[4];
	Cell **adj_cells;
	Cell *c_ptr;
	for (int i = -1; i < 2; i++) {
		for (int j = -1; j < 2; j++) {
			adj_y = y + i;
			adj_x = x + j;
			if (i * j != 0 || (i == 0 && j == 0)) {
				continue;
			}
			if (adj_y >= 0 && adj_x >= 0 && adj_y < 9 && adj_x < 9 &&
			    i * j == 0 && (i != 0 || j != 0) &&
			    G->board[adj_y][adj_x] == 0)
			{
				c_ptr = malloc(sizeof (Cell));
				c_ptr->y = adj_y;
				c_ptr->x = adj_x;
				A[(*adj_cells_n)++] = c_ptr;
			}
		}
	}
	adj_cells = calloc(*adj_cells_n, sizeof(Cell *));
	for (int i = 0; i < *adj_cells_n; i++) {
		adj_cells[i] = A[i];
	}
	return adj_cells;
}

// cityblock metric
int dist(int from_y, int from_x, int to_y, int to_x)
{
	return abs(from_y - to_y) + abs(from_x - to_x);
}

int compare_cells(const void *a, const void *b, void *G)
{
	int d_a = dist((*((Cell **) a))->y, (*((Cell **) a))->x,
			((Game *) G)->cur_cell.y, ((Game *) G)->cur_cell.x);
	int d_b = dist((*((Cell **) b))->y, (*((Cell **) b))->x,
			((Game *) G)->cur_cell.y, ((Game *) G)->cur_cell.x);
	return d_a - d_b;
}

void free_adj_cells(Cell **A, int n)
{
	for (int i = 0; i < n; i++) {
		free(A[i]);
	}
	free(A);
}

bool path_exists(Game *G, int from_y, int from_x, int to_y, int to_x, bool visited[9][9], int seqn)
{
	/* put_symbol(from_y, from_x, seqn + '0'); */
	if (from_y == to_y && from_x == to_x) {
		return true;
	}
	visited[from_y][from_x] = true;
	int adj_cells_n = 0;
	Cell **adj_cells = get_adj_cells(G, from_y, from_x, &adj_cells_n);
	qsort_r(adj_cells, adj_cells_n, sizeof(Cell*), compare_cells, G);
	for (int i = 0; i < adj_cells_n; i++) {
		int next_y = adj_cells[i]->y;
		int next_x = adj_cells[i]->x;
		if (visited[next_y][next_x]) {
			continue;
		}
		if (path_exists(G, next_y, next_x, to_y, to_x, visited, seqn+1)) {
			free_adj_cells(adj_cells, adj_cells_n);
			return true;
		}
	}
	free_adj_cells(adj_cells, adj_cells_n);
	return false;
}

void remove_brick(Game *G, int y, int x, bool draw_border)
{
	G->board[y][x] = 0;
	fill_cell(y, x, COLOR_PAIR(1));
	if (draw_border) 
		draw_cell_border(y, x, COLOR_PAIR(4));
}

bool move_selected_brick(Game *G, int y, int x)
{
	bool visited[9][9];
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			visited[i][j] = false;
		}
	}
	if (path_exists(G, G->cur_brick.y, G->cur_brick.x, y, x, visited, 0)) {
		put_brick(G, y, x, G->board[G->cur_brick.y][G->cur_brick.x]);
		remove_brick(G, G->cur_brick.y, G->cur_brick.x, false);
		deselect_cell(G->cur_brick.y, G->cur_brick.x);
		G->cur_brick.y = -1;
		G->cur_brick.x = -1;
		select_cell(G, y, x);
		return true;
	} else {
		return false;
	}
}

// TODO: add "luck" score: increases when a line reduces due to the popped up
// brick
// TODO: records db
void score(Game *G, int n)
{
	G->score += (int) pow((double) 10, (double) n - 5);
	wmove(INFO_WIN, 1, 1);
	wclrtoeol(INFO_WIN);
	wprintw(INFO_WIN, "SCORE: %d", G->score);
	wrefresh(INFO_WIN);
}

void reduce_bricks(Game *G, int at_y, int at_x)
{
	// TODO: royal flush and the like: 10^42 points for arranging bricks in
	// rainbow order!
	// TODO: slow the animation a bit

	int match_n, y, x, start_i, stop_i;

	Cell axes[] = { { 1, 0 }, { 0, 1 }, { 1, 1 }, { 1, -1 } };
	int dir[] = { -1, 1 };
	match_n = 1;

	for (int a = 0; a < 4; a++) {
		for (int d = 0; d < 2; d++) {
			for (int i = 1; i < 9; i++) {
				y = at_y + axes[a].y * dir[d] * i;
				x = at_x + axes[a].x * dir[d] * i;
				if (y > 9 || y < 0 || x > 9 || x < 0 ||
						G->board[y][x] != G->board[at_y][at_x]) {
					if (dir[d] == -1) {
						start_i = i;
					} else {
						stop_i = i;
					}
					break;
				}
				match_n++;
			}
		}
		if (match_n >= LINE_LEN) {
			score(G, match_n);
			for (int d = 0; d < 2; d++) {
				for (int i = 0; i < (dir[d] == -1 ? start_i : stop_i); i++) {
					y = at_y + axes[a].y * dir[d] * i;
					x = at_x + axes[a].x * dir[d] * i;
					remove_brick(G, y, x, false);
				}
			}
		}
		match_n = 1;
	}

}

int randint(int n)
{
	return floor((rand() / (double) RAND_MAX) * n);
}

void put_bricks_random(Game *G, int n)
{
	Cell free_cells[9*9];	// feels bad but not a big deal
	int n_free_cells = 0;
	int cell_idx = 0;
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			if (G->board[i][j] == 0) {
				Cell c = { i, j };
				free_cells[n_free_cells++] = c;
			}
		}
	}
	for (int i = 0; i < n && n_free_cells > 0; i++, n_free_cells--) {
		cell_idx = randint(n_free_cells);
		put_brick(G, free_cells[cell_idx].y, free_cells[cell_idx].x, randint(6)+1);
		reduce_bricks(G, free_cells[cell_idx].y, free_cells[cell_idx].x);
		free_cells[cell_idx] = free_cells[n_free_cells-1];
	}
}

int get_n_free_cells(Game *G)
{
	// for big boards this should be replaced with a global counter
	int n_free_cells = 0;
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			n_free_cells += (G->board[i][j] == 0);
		}
	}
	return n_free_cells;
}

void die(int status)
{
	endwin();
	exit(status);
}
