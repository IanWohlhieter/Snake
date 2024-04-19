// Ian Wohlhieter and Peter Vanturas
// CS-0355-02

// Compile with "gcc -o snake snake.c -lncurses"
// Execute with "./snake"
// Quit game by pressing Ctrl-C or k

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <curses.h>

struct Node {
    int y;
    int x;
    struct Node *next;
};

// Represent the segments of the snake's body as Nodes of a SLL
// and display the body (from head to tail)
void start_snake(int y_coord, int x_coord, int snake_length, struct Node **snake_boi){
    (*snake_boi)->y = y_coord;
    (*snake_boi)->x = x_coord;
    move(y_coord, x_coord); // display the head
    addch(ACS_BLOCK);
    struct Node *prev = *snake_boi;
    for (int p=1; p<snake_length; p++){
		struct Node *current = (struct Node*) malloc(sizeof(struct Node));
        current->y = y_coord;
        current->x = x_coord-p;
        move(current->y, current->x); // display the new body segment
		addch(ACS_BLOCK);
        prev->next = current; // attach the Node to the tail 
        prev = current; // traverse towards the tail
    }
    prev->next = NULL; // to avoid a "hanging tail"
}

// Attach, display, and reassign the snake's new head, erase and release its old tail
void print_snake(int y_coord, int x_coord, struct Node **head){
    struct Node *new_head = (struct Node*) malloc(sizeof(struct Node)); 
    new_head->y = y_coord;
    new_head->x = x_coord;
    new_head->next = *head; // attach 
    move(new_head->y, new_head->x); //display
    addch(ACS_BLOCK); 
    *head = new_head; // reassign 
    struct Node *prev, *tail = *head;
    while (tail->next != NULL){ // advance to the pre-tail
        prev = tail;
        tail = tail->next;
    }
    move(tail->y, tail->x); // erase
    addch(' ');
    prev->next = NULL; // detach and release
    free(tail);
}

int main(int ac, char *av[]){
    initscr();
    int lines = LINES; // to circumvent window resizing mid-game
    int cols = COLS;
    cbreak();
    curs_set(0);
    refresh();
    printf("Press any key to begin!\r\n");
    getch();
    clear();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
	noecho();
    
    int snake_length = 5;
	int x_coord = 1+snake_length;
	int y_coord = 1;
	int direction = KEY_RIGHT;
    int sleepy_time = 150000;
    
    // Draw the snake pit and the starting snake
    border(ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
    struct Node *snake_boi = (struct Node*) malloc(sizeof(struct Node)); 
    start_snake(y_coord, x_coord, snake_length, &snake_boi);
    refresh();

    // while (snake head is within bounds of pit)
	while (0<x_coord && x_coord<cols-1 && 0<y_coord && y_coord<lines-1){
	    print_snake(y_coord, x_coord, &snake_boi);
	    refresh();
        usleep(sleepy_time);
	
		int curr_dir = direction;
		if ((direction = getch())==ERR) direction = curr_dir;
		switch (direction){
			case 's': direction = KEY_DOWN;
			case KEY_DOWN:
				y_coord++;
				sleepy_time = 225000; // 1.5x sleep (2/3 speed) to compensate for vertical spacing
				break;
			case 'w': direction = KEY_UP;
			case KEY_UP:
				y_coord--;
				sleepy_time = 225000; // 1.5x sleep (2/3 speed) to compensate for vertical spacing
				break;
			case 'a': direction = KEY_LEFT;
			case KEY_LEFT:
				x_coord--;
				sleepy_time = 150000;
				break;
			case 'd': direction = KEY_RIGHT;
			case KEY_RIGHT:
				x_coord++;
				sleepy_time = 150000;
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
