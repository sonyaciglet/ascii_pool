#include <vector>
#include <utility>
#include <ranges>
#include <iostream>
#include <unordered_map>
#include <ncurses.h>
#include <tuple>

#include "config.h"
#include "colors.h"

using std::vector;

class TPlayingField {
 private:
    using TFieldCell= char;
    using TField = vector<vector<TFieldCell>>;
    using TColorField = vector<vector<TColorPair>>;
    using TBallId = int;
    TBallId CUE_BALL_ID = 0;

    enum EBallType {
        Striped,
        Solid,
        CueBall,
    };

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

    const std::unordered_map<EColor, TColorPair> COLOR_MAP = {
        {EColor::Yellow, TColorPair(0b110, 0b110)},
        {EColor::Blue, TColorPair(0b011, 0b011)},
        {EColor::Red, TColorPair(0b100, 0b100)},
        {EColor::Purple, TColorPair(0b101, 0b101)},
        {EColor::Orange, TColorPair(0b110, 0b100)},
        {EColor::Green, TColorPair(0b1010, 0b010)},
        {EColor::Maroon, TColorPair(0b001, 0b001)},
        {EColor::Black, TColorPair(0b1000, 0b000)},
        {EColor::White, TColorPair(0b111, 0b111)},
    };

    class Position {
     public:
        int16_t x;
        int16_t y;
     public:
        Position(int val_x, int val_y): x(val_x), y(val_y) {}
    };

    const uint8_t BALL_COUNT = 16;
    TField field;
    TColorField color_field;
    vector<Position> balls;
    vector<Position> pockets = {
        Position(CONFIG.POCKET_RADIUS - 1, CONFIG.POCKET_RADIUS - 1),
        Position(CONFIG.WIDTH - CONFIG.POCKET_RADIUS, CONFIG.POCKET_RADIUS - 1),
        Position(CONFIG.WIDTH - CONFIG.POCKET_RADIUS, CONFIG.HEIGHT - CONFIG.POCKET_RADIUS),
        Position(CONFIG.POCKET_RADIUS - 1, CONFIG.HEIGHT - CONFIG.POCKET_RADIUS),
        Position(CONFIG.POCKET_RADIUS - 1, CONFIG.HEIGHT / 2),
        Position(CONFIG.WIDTH - CONFIG.POCKET_RADIUS, CONFIG.HEIGHT / 2),
    };

 public:
    EBallType GetBallType(TBallId ball_id) const {
        if (ball_id == CUE_BALL_ID) {
            return EBallType::CueBall;
        } else if (ball_id >= 1 && ball_id < 9) {
            return EBallType::Solid;
        } else if (ball_id >= 9 && ball_id < 16) {
            return EBallType::Striped;
        } else {
            std::unreachable();
        }
    }

    EColor GetBallColor(TBallId ball_id) const {
        if (ball_id == CUE_BALL_ID) {
            return EColor::White;
        }
        auto color_id = (ball_id - 1) % 8;
        if (color_id == 0) return EColor::Yellow;
        else if (color_id == 1) return EColor::Blue;
        else if (color_id == 2) return EColor::Red;
        else if (color_id == 3) return EColor::Purple;
        else if (color_id == 4) return EColor::Orange;
        else if (color_id == 5) return EColor::Green;
        else if (color_id == 6) return EColor::Maroon;
        else if (color_id == 7) return EColor::Black;
        std::unreachable();
    }

    TPlayingField()
        : field(vector<vector<TFieldCell>>(CONFIG.HEIGHT, vector<TFieldCell>(CONFIG.WIDTH, '.')))
        , color_field(vector<vector<TColorPair>>(CONFIG.HEIGHT, vector<TColorPair>(CONFIG.WIDTH, TColorPair(0b1100, 0b010))))
        , balls(vector<Position>(BALL_COUNT, Position(0, 0)))
    {
        // TODO: randomize it
        vector<int> ball_order = {1, 9, 10, 2, 8, 3, 11, 4, 12, 13, 5, 14, 6, 15, 7};
        Position head_position(CONFIG.WIDTH / 2, CONFIG.HEIGHT - 3 * (CONFIG.HEIGHT / 5));
        size_t idx = 0;
        for (size_t row_number = 0; row_number < 5; ++row_number) {
            Position cur_position = head_position;
            cur_position.y -= row_number * CONFIG.BALL_RADIUS * 2;
            cur_position.x -= int(CONFIG.BALL_RADIUS * row_number);
            for (size_t col_number = 0; col_number <= row_number; ++col_number) {
                auto cur_ball = ball_order[idx];
                balls[cur_ball] = cur_position;

                idx++;
                cur_position.x += 2 * CONFIG.BALL_RADIUS;
            }
        }
        balls[CUE_BALL_ID] =  Position(CONFIG.WIDTH / 2, 2 * CONFIG.HEIGHT / 3);

        // Draw the border
        for (int row = 0; row < CONFIG.HEIGHT; ++row) {
            for (int dy = -CONFIG.BORDER_THICKNESS; dy <= 0; ++dy) {
                field[row][CONFIG.BORDER_THICKNESS + dy] = '@';
                color_field[row][CONFIG.BORDER_THICKNESS + dy] = TColorPair(0b110, 0b100);
                field[row][CONFIG.WIDTH - 1 + dy] = '@';
                color_field[row][CONFIG.WIDTH - 1 + dy] = TColorPair(0b110, 0b100);
            }
        }
        for (int col = 0; col < CONFIG.WIDTH; ++col) {
            for (int dx = -CONFIG.BORDER_THICKNESS; dx <= 0; ++dx) {
                field[CONFIG.BORDER_THICKNESS + dx][col] = '@';
                color_field[CONFIG.BORDER_THICKNESS + dx][col] = TColorPair(0b110, 0b100);
                field[CONFIG.HEIGHT - 1 + dx][col] = '@';
                color_field[CONFIG.HEIGHT - 1 + dx][col] = TColorPair(0b110, 0b100);
            }
        }

        // Draw pockets
        // TODO make an enum with all char mappings
        // Top left
        for (int dx = -CONFIG.POCKET_RADIUS; dx <= CONFIG.POCKET_RADIUS; ++dx) {
            for (int dy = -CONFIG.POCKET_RADIUS; dy <= CONFIG.POCKET_RADIUS; ++dy) {
                for (auto pocket_pos : pockets) {
                    int x = std::min(std::max(1, pocket_pos.x + dx), CONFIG.WIDTH - 2);
                    int y = std::min(std::max(1, pocket_pos.y + dy), CONFIG.HEIGHT - 2);
                    if (field[y][x] == '@') {
                        field[y][x] = '*';
                        color_field[y][x] = COLOR_MAP.at(EColor::Black);
                    }
                    // check that coords are in border
                    /*
                    bool is_left_border = x <= CONFIG.BORDER_THICKNESS;
                    bool is_right_border = x >= (CONFIG.WIDTH - CONFIG.BORDER_THICKNESS);
                    bool is_top_border = y <= CONFIG.BORDER_THICKNESS;
                    bool is_bottom_border = y >= (CONFIG.HEIGHT - CONFIG.BORDER_THICKNESS);
                    if (is_left_border || is_right_border || is_top_border || is_bottom_border) {
                        field[y][x] = '*';
                        color_field[y][x] = COLOR_MAP.at(EColor::White);
                    }
                    */
                }
            }
        }


    }

    std::tuple<TField, TColorField> DrawBall(TBallId ball_id) const {
        // TField res = vector<vector<TFieldCell>>(CONFIG.BALL_RADIUS * 2, vector<TFieldCell>(CONFIG.BALL_RADIUS * 2, 'A' + ball_id));
        std::cerr << "Drawing ball" << ball_id << std::endl;
        TField res = vector<vector<TFieldCell>>(CONFIG.BALL_RADIUS * 2, vector<TFieldCell>(CONFIG.BALL_RADIUS * 2, ACS_BLOCK));
        const auto ball_color = COLOR_MAP.at(GetBallColor(ball_id));
        TColorField res_color = vector<vector<TColorPair>>(CONFIG.BALL_RADIUS * 2, vector<TColorPair>(CONFIG.BALL_RADIUS * 2, ball_color));
        if (GetBallType(ball_id) == EBallType::Striped) {
            for (int i = 0; i < res.size(); ++i) {
                res_color[i][res.size() - i - 1] = COLOR_MAP.at(EColor::White);
                // res[i][res.size() - i - 1] = '/';
            }
        }
        std::cerr << "Drew ball" << ball_id << std::endl;
        return {res, res_color};
    }

    std::tuple<TField, TColorField> BuildFieldMatrix() const {
        // TODO: this is dumb, probably don't need to keep field in memory at all
        auto new_field = field;
        auto new_color_field = color_field;
        for (size_t ball_id = 0; ball_id < balls.size(); ++ball_id) {
            auto ball_pos = balls[ball_id];
            // iterate over [-radius, radius) just so everything looks square and nice
            auto [char_mat, color_mat] = DrawBall(ball_id);
            for (size_t row_idx = 0; row_idx < char_mat.size(); ++row_idx) {
                for (size_t col_idx = 0; col_idx < char_mat[0].size(); ++col_idx) {
                    new_field[ball_pos.y + row_idx][ball_pos.x + col_idx] = char_mat[row_idx][col_idx];
                    new_color_field[ball_pos.y + row_idx][ball_pos.x + col_idx] = color_mat[row_idx][col_idx];
                }
            }
        }
        return {new_field, new_color_field};
    }
};
