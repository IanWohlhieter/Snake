// Ian Wohlhieter and Peter Vanturas
// 04-25-2024
// CS-355-02

// Compile with "gcc -o snake snake.c -lncurses"
// Execute with "./snake"
// Quit game by pressing Ctrl-C

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <curses.h>
#include <string.h>
#include <time.h>
#include <math.h>

// Nodes to make a SLL
// Snake and trophies will be represented as Nodes 
// (value/lifespan unused for the snake but easier to combine struct)
struct Node {
    int y;
    int x;
    int value;
    int lifespan;
    struct Node *next;
};

// Add the snake's new head, delete its old tail
void print_snake(int y_coord, int x_coord, struct Node **snake_body, int *snake_food, int *snake_length){
    attron(COLOR_PAIR(1));
    struct Node *new_head = (struct Node*) malloc(sizeof(struct Node)); 
    new_head->y = y_coord;
    new_head->x = x_coord;
    new_head->next = *snake_body; // attach 
    mvaddch(new_head->y, new_head->x, ACS_BLOCK); //display
    *snake_body = new_head; // reassign 
    if ((*snake_food) <= 0){
        struct Node *prev, *tail = *snake_body;
        while (tail->next != NULL){ // traverse
            prev = tail;
            tail = tail->next;
        }
        mvaddch(tail->y, tail->x, ' '); // erase
        prev->next = NULL; // detach
        free(tail); // release
    }
    else {
        --(*snake_food);
        ++(*snake_length);
    }
}

// Make a "new" trophy (same Node, different data)
// Ensure that it is not on top of the snake
void make_new_trophy(int lines, int cols, struct Node **trophy, struct Node **snake_body){
    attron(COLOR_PAIR(2));
    bool trophy_on_snake = false;
    do {
        struct Node *body_pointer = *snake_body;
        trophy_on_snake = false;
        (*trophy)->y = (rand()%(lines-2))+1;
        (*trophy)->x = (rand()%(cols-2))+1;
        (*trophy)->value = (rand()%9)+1;
        (*trophy)->lifespan = (rand()%9)+1;
        while (body_pointer != NULL){
            if ((*trophy)->x==body_pointer->x && (*trophy)->y==body_pointer->y){
                trophy_on_snake = true;
                break;
            }
            body_pointer = body_pointer->next;
        }
    } while (trophy_on_snake);
    char buf[2] = "0\0";
    sprintf(buf, "%d", (*trophy)->value);
    mvaddnstr((*trophy)->y, (*trophy)->x, buf, 2);
}

// Check if the snake head is eating any of its body segments
bool collision_check(struct Node** snake_body){
    bool collision = false;
    struct Node *head = *snake_body;
    struct Node *body = head->next->next;
    while (body != NULL){
        if (head->x==body->x && head->y==body->y){
            return (collision = true);
        }
        body = body->next;
    }
    return collision;
}

// Pick a random start location and move in one of two directions towards the center
int random_start(int lines, int cols, int *x_coord, int*y_coord){
    *y_coord = (rand()%(lines-2))+1;
    *x_coord = (rand()%(cols-2))+1;
    int random_dir = rand()%2;
    if (*y_coord<lines/2 && *x_coord<cols/2) {if (random_dir) {return KEY_RIGHT;} else return KEY_DOWN;};
    if (*y_coord<lines/2 && *x_coord>cols/2) {if (random_dir) {return KEY_LEFT;} else return KEY_DOWN;};
    if (*y_coord>lines/2 && *x_coord<cols/2) {if (random_dir) {return KEY_RIGHT;} else return KEY_UP;};
    if (*y_coord>lines/2 && *x_coord>cols/2) {if (random_dir) {return KEY_LEFT;} else return KEY_UP;};
    return 0;
}

// Free all memory that was allocated for the snake body and trophy
void free_all(struct Node **snake_body, struct Node **trophy){
    struct Node *current = *snake_body;
        while (current != NULL){
            struct Node *prev = current;
            current = current->next;
            free(prev); 
        }
    snake_body = NULL;
    free(*trophy);
    *trophy = NULL;
}

int main(int ac, char *av[]){
    initscr();
    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_WHITE, COLOR_RED);
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
    
    int snake_length = 0;
    int snake_food = 5;
	int x_coord;
	int y_coord;
    srand(time(0)); 
	int direction = random_start(lines, cols, &x_coord, &y_coord);
    int safe_directions[8] = {'w','a','s','d',KEY_UP,KEY_DOWN,KEY_LEFT,KEY_RIGHT};
    int sleepy_time = 120000;
    int speed_penalty = 0;
    
    // Draw the snake pit and allocate space for the snake 
    border(ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
    struct Node *snake_body = (struct Node*) malloc(sizeof(struct Node)); 
    
    // Make first trophy and track 
    struct Node *trophy = (struct Node*) malloc(sizeof(struct Node)); 
    make_new_trophy(lines, cols, &trophy, &snake_body);
    time_t trophy_time, time_passed;
    time(&trophy_time);
    refresh();

    // while (snake head is within bounds of pit), and (win condition has not been reached)
	while (0<x_coord && x_coord<cols-1 && 0<y_coord && y_coord<lines-1 && snake_food<win_length){
        
        print_snake(y_coord, x_coord, &snake_body, &snake_food, &snake_length);
        refresh();
        if (collision_check(&snake_body)) break;

        if (snake_body->x==trophy->x && snake_body->y==trophy->y){ // eat trophy
            snake_food += trophy->value;
            speed_penalty += (trophy->value)*300;
            make_new_trophy(lines, cols, &trophy, &snake_body);
            time(&trophy_time);
        }
        time(&time_passed);
        if (time_passed-trophy_time > (trophy->lifespan)){ // check trophy expiration
            mvaddch(trophy->y, trophy->x, ' ');
            make_new_trophy(lines, cols, &trophy, &snake_body);
            time(&trophy_time);
        }

		int curr_dir = direction;
        direction = getch();
        bool is_safe_direction = false;
        for (int i=0; i<(sizeof(safe_directions)/sizeof(int)); i++) {
            is_safe_direction |= (direction == safe_directions[i]);
        }
        if (!is_safe_direction) direction = curr_dir;
        switch (direction){
            case 's': direction = KEY_DOWN;
            case KEY_DOWN:
                y_coord++;
                sleepy_time = 165000; // 1.5x sleep (2/3 speed) to compensate for vertical spacing
                break;
            case 'w': direction = KEY_UP;
			case KEY_UP:
                y_coord--;
                sleepy_time = 165000; // 1.5x sleep (2/3 speed) to compensate for vertical spacing
                break;
            case 'a': direction = KEY_LEFT;
            case KEY_LEFT:
                x_coord--;
                sleepy_time = 110000;
                break;
            case 'd': direction = KEY_RIGHT;
            case KEY_RIGHT:
                x_coord++;
                sleepy_time = 110000;
                break;
            case 'k':
                endwin();
                return 0;
            default:
                direction = curr_dir;
                break;
        }
        usleep(sleepy_time-speed_penalty);
    }

    free_all(&snake_body, &trophy);

    timeout(10000);
    clear();
    refresh();
    if (snake_length>=win_length) printf("Congrats! You won!\r\n\nPress any key to exit:\r\n");
    else printf("Uh oh! You lost!\r\n\nPress any key to exit...\r\n");
    getch();
    clear();
    endwin();
    return 0;
}