// Ian Wohlhieter and Peter Vanturas
// CS-0355-02

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <curses.h>

int main(int ac, char *av[]){
	int snake_length = 5;
    initscr();
    cbreak();
	curs_set(0); // sets the cursor to invisible (0-2)=(invis-highvis)
    printf("The screen is %d rows, by %d columns.\n", LINES-1, COLS);
    getchar();
    clear();
	nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
	noecho();
	int x_coord = 1+snake_length;
	int y_coord = 1;
	int direction = KEY_RIGHT;
	while (0<x_coord && x_coord<COLS && 0<y_coord && y_coord<LINES){ // while (within bounds of pit)
		clear();
		border(ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
        move(y_coord, x_coord);
        for (int p=0; p<snake_length; p++){ // print the snake (from head to tail)
			move(y_coord, x_coord+snake_length-p);
			addch(ACS_BLOCK);
		}
        refresh();
		int curr_dir = direction;
		usleep(10000);
        direction = getch();
        switch (direction){
            case KEY_DOWN: // 402
                y_coord++;
                break;
			case KEY_UP: // 403
                y_coord--;
                break;
            case KEY_LEFT: // 404
                x_coord--;
                break;
            case KEY_RIGHT: // 405
                x_coord++;
                break;
            case 'k':
                endwin();
                return 0;
            default:
				direction = curr_dir;
                break;
        }
    }
    getchar();
    endwin();
    return 0;
}
