// Ian Wohlhieter and Peter Vanturas
// CS-0355-02

// compile with "gcc snake.c -lncurses"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <curses.h>

struct Node {
  int y;
  int x;
  struct Node *next;
};

// still need to make and print the starting snake
struct Node* print_snake(struct Node **head, int y_coord, int x_coord){
    move(y_coord, x_coord);
    addch(ACS_BLOCK); // add block at position of new head
    struct Node *curr_node = (struct Node*) malloc(sizeof(struct Node)); // make new node with y and x "coordinates"
    curr_node->y = y_coord;
    curr_node->x = x_coord;
    curr_node->next = *head; // point from new head to the old head
    struct Node *prev, *tail = curr_node;
    while (tail->next != NULL){ // advance to the pre-tail of the snake
        prev = tail;
        tail = tail->next;
    }
    move(tail->y, tail->x); // erase the block at the (y,x) of the tail
    addch(' ');
    prev->next = NULL; // remove and release the tail
    free(tail);
    return curr_node;
}

struct Node* start_snake(int y_coord, int x_coord, int snake_length, struct Node **snake_boi){
    // struct Node *head = (struct Node*) malloc(sizeof(struct Node)); // make new node with y and x "coordinates"
    (*snake_boi)->y = y_coord;
    (*snake_boi)->x = x_coord;
    move(y_coord, x_coord);
    addch(ACS_BLOCK);
    struct Node *prev = *snake_boi;
    for (int p=1; p<snake_length; p++){ // print the snake (from head to tail)
		struct Node *current = (struct Node*) malloc(sizeof(struct Node)); // make a new node
        current->y = y_coord;
        current->x = x_coord-p;
        move(current->y, current->x);
		addch(ACS_BLOCK);
        prev->next = current; // attach it to the head
        prev = current; // move the "head" towards the tail
    }
    prev->next = NULL;
    return *snake_boi;
}

int main(int ac, char *av[]){
	int snake_length = 5;
    initscr();
    cbreak();
    refresh();
    printf("The screen is %d rows, by %d columns.\r\nPress any key to begin!", LINES-1, COLS);
    getch();
    clear();
    curs_set(0);
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
	noecho();
	int x_coord = 1+snake_length;
	int y_coord = 1;
	int direction = KEY_RIGHT;
    
    border(ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
    struct Node *snake_boi = (struct Node*) malloc(sizeof(struct Node));
    snake_boi = start_snake(y_coord, x_coord, snake_length, &snake_boi); // initialize that boi
    refresh();
	while (0<x_coord && x_coord<COLS-1 && 0<y_coord && y_coord<LINES-1){ // while (snake head is within bounds of pit)
		int curr_dir = direction;
        
        snake_boi = print_snake(&snake_boi, y_coord, x_coord); // NEW PRINT METHOD
        refresh();
		usleep(100000);

        if ((direction = getch())==ERR) direction = curr_dir;
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
    endwin();
    return 0;
}
