#include <iostream>
#include <ncurses.h>

int main() {
    initscr();
    printw("Hello, World!"); // Print "Hello, World!"
    refresh(); // Refresh to show the output
    getch(); // Wait for user input
    endwin(); // End ncurses mode
    return 0;
}
