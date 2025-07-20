#pragma once

#include <ncurses.h>

class TWindowCursor{
 private:
    int16_t x = 0;
    int16_t y = 0;
    WINDOW* window_ptr;

 public:
    TWindowCursor(WINDOW* window_ptr) : window_ptr(window_ptr) {
    }

    void set_position() {
        // 1 is added to offset the box border
        wmove(window_ptr, y + 1, x + 1);
    }

    void set_position(int16_t new_x, int16_t new_y) {
        x = new_x;
        y = new_y;
    }
    
    bool move_left() {
        if (x > 0) {
            --x;
            set_position();
            return true;
        } else {
            return false;
        }
    }

    bool move_right() {
        if (x < CONFIG.WIDTH - 1) {
            ++x;
            set_position();
            return true;
        } else {
            return false;
        }
    }

    bool move_up() {
        if (y > 0) {
            --y;
            set_position();
            return true;
        } else {
            return false;
        }
    }

    bool move_down() {
        if (y < CONFIG.HEIGHT - 1) {
            ++y;
            set_position();
            return true;
        } else {
            return false;
        }
    }
};
