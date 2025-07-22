#pragma once

inline struct Config {
    uint8_t WIDTH = 80;
    uint8_t HEIGHT = 50;
    char BOX_CHAR = '@';

    // TODO make this a radius
    int8_t BALL_RADIUS = 2;
    int8_t POCKET_RADIUS = 3;
    // TODO FIX IT SO 0 border is possible
    uint8_t BORDER_THICKNESS = 1;
} CONFIG;
