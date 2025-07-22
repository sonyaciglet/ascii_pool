#pragma once

#include <ncurses.h>

#include <utility>

enum EColor {
    Yellow,
    Blue,
    Red,
    Purple,
    Orange,
    Green,
    Maroon,
    Black,
    White,
};

inline short curs_color(int fg)
{
    switch (7 & fg) {           /* RGB */
    case 0:                     /* 000 */
        return (COLOR_BLACK);
    case 1:                     /* 001 */
        return (COLOR_BLUE);
    case 2:                     /* 010 */
        return (COLOR_GREEN);
    case 3:                     /* 011 */
        return (COLOR_CYAN);
    case 4:                     /* 100 */
        return (COLOR_RED);
    case 5:                     /* 101 */
        return (COLOR_MAGENTA);
    case 6:                     /* 110 */
        return (COLOR_YELLOW);
    case 7:                     /* 111 */
        return (COLOR_WHITE);
    default:
        std::unreachable();
    }
}

inline int colornum(int fg, int bg)
{
    int B, bbb, ffff;

    B = 1 << 7;
    bbb = (7 & bg) << 4;
    ffff = 7 & fg;

    return (B | bbb | ffff);
}

class TColorPair{
 public:
    uint8_t foreground;
    uint8_t background;
    TColorPair(uint8_t fg, uint8_t bg): foreground(fg), background(bg) {}
};

const std::unordered_map<EColor, TColorPair> COLOR_MAP = {
    {EColor::Yellow, TColorPair(0b110, 0b110)},
    {EColor::Blue, TColorPair(0b011, 0b011)},
    {EColor::Red, TColorPair(0b100, 0b100)},
    {EColor::Purple, TColorPair(0b101, 0b101)},
    {EColor::Orange, TColorPair(0b110, 0b100)}, {EColor::Green, TColorPair(0b1010, 0b010)},
    {EColor::Maroon, TColorPair(0b001, 0b001)},
    {EColor::Black, TColorPair(0b1000, 0b000)},
    {EColor::White, TColorPair(0b111, 0b111)},
};

inline void init_colorpairs(void) {
    int fg, bg;
    int colorpair;

    for (bg = 0; bg <= 7; bg++) {
        for (fg = 0; fg <= 7; fg++) {
            colorpair = colornum(fg, bg);
            init_pair(colorpair, curs_color(fg), curs_color(bg));
        }
    }
}

inline void setcolor(WINDOW* win, TColorPair colorPair) {
    wattron(win, COLOR_PAIR(colornum(colorPair.foreground, colorPair.background)));
}

inline void unsetcolor(WINDOW* win, TColorPair colorPair) {
    wattroff(win, COLOR_PAIR(colornum(colorPair.foreground, colorPair.background)));
}
