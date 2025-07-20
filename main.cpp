#include <ncurses.h>
#include <memory>
#include <algorithm>

#include "config.h"
#include "field.h"
#include "colors.h"
#include "cursor.h"


int main() {
    setlocale(LC_CTYPE,"C-UTF-8");
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    if (has_colors() == FALSE) {
        std::cerr << "doesnt work" << std::endl;
        exit(1);
    } else {
        std::cerr << "works" << std::endl;
    }
    auto win = newwin(CONFIG.HEIGHT, CONFIG.WIDTH, 1, 1);
    touchwin(win);
    wrefresh(win); // Refresh to show the box
    refresh();

    TWindowCursor cursor(win);
    TPlayingField playingField;
    auto [field_matrix, color_matrix] = playingField.BuildFieldMatrix();
    std::cerr << "drawing field" << std::endl;

    start_color();
    init_colorpairs();
    int ch;
    while((ch = getch()) != 'q') {
        int counter = 0;
        for (int row_idx = 0; row_idx < field_matrix.size(); ++row_idx) {
            for (int col_idx = 0; col_idx < field_matrix[0].size(); ++col_idx) {
                auto color = color_matrix[row_idx][col_idx];
                setcolor(win, TColorPair(color));
                // wprintw(win, "%c", elem);
                auto elem = field_matrix[row_idx][col_idx];
                mvwaddch(win, row_idx, col_idx, elem);
                unsetcolor(win, TColorPair(color));
                counter++;
            }
            std::cerr << std::endl;
            wrefresh(win);
        }
    }

    /*
    int ch;
    bool once = false;
    wrefresh(win); // Refresh to show the box
        if (!once) {
            once = true;
        }
        switch (ch) {
            case KEY_LEFT:
                cursor.move_left();
                break;
            case KEY_RIGHT:
                cursor.move_right();
                break;
            case KEY_UP:
                cursor.move_up();
                break;
            case KEY_DOWN:
                cursor.move_down();
                break;
        }
        wrefresh(win);
    }
    */
    endwin(); // End ncurses mode
    return 0;
}
