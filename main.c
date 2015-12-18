#include "lines.h"

WINDOW *DEBUG_WIN, *BOARD_WIN, *INFO_WIN, *HELP_WIN;

int main()
{
	initscr();

	if (has_colors() == FALSE) {
		endwin();
		printf("Your terminal does not support color\n");
		exit(EXIT_FAILURE);
	}

	int ch, startx, starty, board_win_width, board_win_height;
	bool end_turn = false;

	start_color();
	cbreak();
	noecho();
	curs_set(0);

	Game G;
	// FIXME: Initialize via {}
	G.cur_cell.x = 0;
	G.cur_cell.y = 0;
	G.cur_brick.x = -1;
	G.cur_brick.y = -1;
	G.score = 0;
	
	board_win_height = CELL_HEIGHT * 9 - 8;
	board_win_width = CELL_WIDTH * 9 - 8;
	startx = (COLS - board_win_width) / 2;
	starty = (LINES - board_win_height) / 2;

	BOARD_WIN = newwin(board_win_height, board_win_width, starty, startx);
	box(BOARD_WIN, 0, 0);
	// FIXME: proper debugging window
	DEBUG_WIN = newwin(6, 120, 0, 0);
	/* box(debug_win, 0, 0); */
	// TODO: show next 3 bricks
	INFO_WIN = newwin(board_win_height, 13,
			starty, startx + board_win_width + 1);
	/* box(INFO_WIN, 0, 0); */
	// TODO: hilite key labels
	HELP_WIN = newwin(5, board_win_width, starty + board_win_height, startx); 
	/* box(HELP_WIN, 0, 0); */
	mvwprintw(HELP_WIN, 1, 1, "Arrow keys or h/j/k/l to move the cursor;");
	mvwprintw(HELP_WIN, 2, 1, "Space to make a selection;");
	mvwprintw(HELP_WIN, 3, 1, "q to quit.");

	wrefresh(BOARD_WIN);
	wrefresh(DEBUG_WIN);
	wrefresh(INFO_WIN);
	wrefresh(HELP_WIN);

	keypad(BOARD_WIN, true);

	init_pair(1, COLOR_BLACK, COLOR_BLACK);
	init_pair(2, COLOR_WHITE, COLOR_BLACK);
	init_pair(3, COLOR_GREEN, COLOR_BLACK);
	init_pair(4, COLOR_RED, COLOR_BLACK);

	// FIXME: replace with a loop
	init_pair(11, COLOR_BLACK, COLOR_RED);
	init_pair(12, COLOR_BLACK, COLOR_GREEN);
	init_pair(13, COLOR_BLACK, COLOR_YELLOW);
	init_pair(14, COLOR_BLACK, COLOR_BLUE);
	init_pair(15, COLOR_BLACK, COLOR_MAGENTA);
	init_pair(16, COLOR_BLACK, COLOR_CYAN);

	// Initialize the board
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			G.board[i][j] = 0;
		}
	}

	select_cell(&G, 0, 0);
	srand(time(NULL));
	score(&G, 0);
	put_bricks_random(&G, N_RANDOM_BRICKS);

	// TODO: add mouse support
	while (get_n_free_cells(&G)) {
		while (true) {
			end_turn = false;
			switch (ch = wgetch(BOARD_WIN)) {
				case 'h':
				case KEY_LEFT:
					select_cell(&G, G.cur_cell.y, G.cur_cell.x - 1);
					break;
				case 'l':
				case KEY_RIGHT:
					select_cell(&G, G.cur_cell.y, G.cur_cell.x + 1);
					break;
				case 'k':
				case KEY_UP:
					select_cell(&G, G.cur_cell.y - 1, G.cur_cell.x);
					break;
				case 'j':
				case KEY_DOWN:
					select_cell(&G, G.cur_cell.y + 1, G.cur_cell.x);
					break;
				case 'q':
					die(EXIT_SUCCESS);
					break;
				case ' ':
					if (G.board[G.cur_cell.y][G.cur_cell.x] != 0) {
						select_brick(&G, G.cur_cell.y, G.cur_cell.x);
					} else if (G.cur_brick.y != -1) { 
						if (move_selected_brick(&G, G.cur_cell.y, G.cur_cell.x)) {
							reduce_bricks(&G, G.cur_cell.y, G.cur_cell.x);
							end_turn = true;
						}
					}
					break;
			}
			if (end_turn) {
				put_bricks_random(&G, N_RANDOM_BRICKS);
				break;
			}
		}

	}

	WINDOW *message_win;
	int msg_height = 4 + 3;
	int msg_width = 70;
	message_win = newwin(msg_height, msg_width,
			(LINES - msg_height) / 2, (COLS - msg_width) / 2);
	box(message_win, 0, 0);
	mvwprintw(message_win, 0, (msg_width - 20) / 2 , " Very well indeed! ");
	mvwprintw(message_win, 2, 2, "You scored %d points.", G.score);
	mvwprintw(message_win, 4, 2, "Have another try.");
	wrefresh(message_win);
	getch();

	die(EXIT_SUCCESS);
}

