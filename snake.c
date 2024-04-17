// Ian Wohlhieter and Peter Vanturas
// CS-355-02

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <curses.h>

int main(int ac, char *av[]){
    initscr();
    printf("The screen is %d rows, by %d columns.\n", LINES-1, COLS);
    getchar();
    clear();
    border(ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
    for (int i=1; i<LINES-1; i++){
        move(i, i*2); // print along diagonal (relative to ULCORNER)
        addstr("Hello, world!\t");
        refresh();
        usleep(1000000);
        move(i, i*2); // go back to same position
        addstr("                "); // overwrite with blanks
    }
    getchar();
    endwin();
    return 0;
}
