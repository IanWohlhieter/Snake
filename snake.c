// Ian Wohlhieter and Peter Vanturas
// CS-355-02
// 04-23-2024

// Compile with "gcc -o snake snake.c -lmath -lncurses"
// Execute with "./snake"
// Quit game by pressing Ctrl-C or k

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <curses.h>
#include <string.h>

// Ian 
// Nodes to make a SLL
struct Node {
    int y;
    int x;
    int value;
    struct Node *next;
};

// Ian
// Represent the segments of the snake's body as Nodes of a SLL
// and display the body (from head to tail)
void start_snake(int y_coord, int x_coord, int snake_length, struct Node **snake_body){
    (*snake_body)->y = y_coord;
    (*snake_body)->x = x_coord;
    move(y_coord, x_coord); // display
    addch(ACS_BLOCK);
    struct Node *prev = *snake_body;
    for (int p=1; p<snake_length; p++){
		struct Node *current = (struct Node*) malloc(sizeof(struct Node));
        current->y = y_coord;
        current->x = x_coord-p;
        move(current->y, current->x); // display
		addch(ACS_BLOCK);
        prev->next = current; // attach
       prev = current; // traverse
    }
    prev->next = NULL; // to avoid a "hanging tail"
}

// Peter
// Add the snake's new head, delete its old tail
void print_snake(int y_coord, int x_coord, struct Node **head, int *snake_food, int *snake_length){
    struct Node *new_head = (struct Node*) malloc(sizeof(struct Node)); 
    new_head->y = y_coord;
    new_head->x = x_coord;
    new_head->next = *head; // attach 
    move(new_head->y, new_head->x); //display
    addch(ACS_BLOCK); 
    *head = new_head; // reassign 
    if ((*snake_food) <= 0){
        struct Node *prev, *tail = *head;
        while (tail->next != NULL){ // traverse
            prev = tail;
            tail = tail->next;
        }
        move(tail->y, tail->x); // erase
        addch(' ');
        prev->next = NULL; // detach
        free(tail); // release
    }
    else {
        --(*snake_food);
        ++(*snake_length);
    }
}

void make_new_trophy(int lines, int cols, struct Node** trophy){
    // struct Node *trophy = (struct Node*) malloc(sizeof(struct Node)); 
    (*trophy)->y = (rand()%(lines-2))+1;
    (*trophy)->x = (rand()%(cols-1))+1;
    usleep((*trophy)->x);
    (*trophy)->value = (rand()%9)+1;
    move((*trophy)->y, (*trophy)->x);
    char buf[2] = "0\0";
    sprintf(buf, "%d", (*trophy)->value);
    addstr(buf);
}

bool collision_check(struct Node** snake_body){
    bool snake_on_snake = false;
    struct Node *current = (*snake_body)->next->next;
    while (current != NULL){
        if ((*snake_body)->x==current->x && (*snake_body)->y==current->y){
            return (snake_on_snake = true);
        }
        current = current->next;
    }
    return snake_on_snake;
}

// Top half Ian 
int main(int ac, char *av[]){
    initscr();
    int lines = LINES; // to circumvent window resizing mid-game
    int cols = COLS;
    int win_length = lines+cols;
    cbreak();
    curs_set(0);
    keypad(stdscr, TRUE);
    refresh();
    printf("Pit is %d lines by %d columns.\r\nPress any key to begin!\r\n", lines, cols);
    getch();
    clear(); 
    nodelay(stdscr, TRUE);
	noecho();
    
    int snake_length = 5;
    int snake_food = 0;
    bool snake_on_snake = false;
	int x_coord = 1+snake_length;
	int y_coord = 1;
	int direction = KEY_RIGHT;
    int safe_directions[8] = {'w','a','s','d',KEY_UP,KEY_DOWN,KEY_LEFT,KEY_RIGHT};
    int sleepy_time = 150000;
    int speed_penalty = 0;
    
    // Next half Peter
    // Draw the snake pit and initialize the starting snake
    border(ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
    struct Node *snake_body = (struct Node*) malloc(sizeof(struct Node)); 
    start_snake(y_coord, x_coord, snake_length, &snake_body);
    // Draw first trophy
    struct Node *trophy = (struct Node*) malloc(sizeof(struct Node)); 
    make_new_trophy(lines, cols, &trophy);
    refresh();

    // while (snake head is within bounds of pit), and win condition has not been reached
	while (0<x_coord && x_coord<cols-1 && 0<y_coord && y_coord<lines-1 && snake_length<win_length){
        
		int curr_dir = direction;
        direction = getch();
        bool is_safe_direction = false;
        for (int i=0; i<8; i++){is_safe_direction |= (direction == safe_directions[i]);}
        if (!is_safe_direction) direction = curr_dir;
        switch (direction){
            case 's': direction = KEY_DOWN;
            case KEY_DOWN:
                y_coord++;
                sleepy_time = 180000; // 1.5x sleep (2/3 speed) to compensate for vertical spacing
                break;
            case 'w': direction = KEY_UP;
			case KEY_UP:
                y_coord--;
                sleepy_time = 180000; // 1.5x sleep (2/3 speed) to compensate for vertical spacing
                break;
            case 'a': direction = KEY_LEFT;
            case KEY_LEFT:
                x_coord--;
                sleepy_time = 120000;
                break;
            case 'd': direction = KEY_RIGHT;
            case KEY_RIGHT:
                x_coord++;
                sleepy_time = 120000;
                break;
            case 'k':
                endwin();
                return 0;
            default:
				direction = curr_dir;
                break;
        }
        print_snake(y_coord, x_coord, &snake_body, &snake_food, &snake_length);
        refresh();
        snake_on_snake = collision_check(&snake_body);
        if (snake_on_snake) break;

        struct Node *body_pointer = snake_body;
        if (x_coord==trophy->x && y_coord==trophy->y){ // eat trophy
            snake_food += trophy->value;
            speed_penalty += (trophy->value)*100;
            bool trophy_on_snake = false;
            do {
                trophy_on_snake = false;
                make_new_trophy(lines, cols, &trophy);
                body_pointer = snake_body;
                while (body_pointer != NULL){
                    if (trophy->x==body_pointer->x && trophy->y==body_pointer->y){
                        trophy_on_snake = true;
                        mvaddch(trophy->y,trophy->x, ACS_BLOCK);
                        break;
                    }
                    body_pointer = body_pointer->next;
                }
            } while (trophy_on_snake);
        }
        usleep(sleepy_time-speed_penalty);
    } // end of while loop

    timeout(10000);
    clear();
    refresh();
    if (snake_length>=win_length) printf("Congrats! You won!\r\n\nPress any key to exit:\r\n");
    else printf("Uh oh! You lost!\r\n\nPress any key to exit:\r\n");
    getch();
    clear();
    endwin();
    return 0;
}
