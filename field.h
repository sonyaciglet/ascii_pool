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
    struct TFieldCell {
        char Character;
        TColorPair Color;

        TFieldCell(char character, TColorPair color): Character(character), Color(color) {}
        TFieldCell(): Character('.'), Color(COLOR_MAP.at(EColor::White)) {}
    };

    using TFieldMatrix = vector<vector<TFieldCell>>;
    using TBallId = int;

    class TVec2d {
     public:
        int16_t x;
        int16_t y;
     public:
        TVec2d(int val_x, int val_y): x(val_x), y(val_y) {}
    };

    using TPosition = TVec2d;
    using TVelocity = TVec2d;

    class TBall {
     private:
        const static TBallId CUE_BALL_ID = 0;
        enum EBallType {
            Striped,
            Solid,
            CueBall,
        };
        size_t Radius_;
        const TBallId Id_;

        TPosition Pos_;
        // TVelocity;

        TFieldMatrix BallSprite_;


     private:
        EBallType GetBallType() const;
        EColor GetBallColor() const;
        void DrawBall();

     public:
        TBall(TBallId ballId, TPosition position=TPosition(0, 0))
            : Radius_(CONFIG.BALL_RADIUS)
            , Id_(ballId)
            , Pos_(position)
            , BallSprite_(TFieldMatrix(Radius_ * 2, std::vector<TFieldCell>(Radius_ * 2)))
        {
            DrawBall();
        }
        bool IsCueBall();
        const TPosition& GetPosition() const {
            return Pos_;
        }
        const TFieldMatrix& GetBallSprite() const {
            return BallSprite_;
        }
    };


    const uint8_t BALL_COUNT = 16;
    TFieldMatrix field;

    vector<TBall> Balls;
    vector<TPosition> pockets = {
        TPosition(CONFIG.POCKET_RADIUS - 1, CONFIG.POCKET_RADIUS - 1),
        TPosition(CONFIG.WIDTH - CONFIG.POCKET_RADIUS, CONFIG.POCKET_RADIUS - 1),
        TPosition(CONFIG.WIDTH - CONFIG.POCKET_RADIUS, CONFIG.HEIGHT - CONFIG.POCKET_RADIUS),
        TPosition(CONFIG.POCKET_RADIUS - 1, CONFIG.HEIGHT - CONFIG.POCKET_RADIUS),
        TPosition(CONFIG.POCKET_RADIUS - 1, CONFIG.HEIGHT / 2),
        TPosition(CONFIG.WIDTH - CONFIG.POCKET_RADIUS, CONFIG.HEIGHT / 2),
    };

 public:

    void AddBorder() {
        for (int row = 0; row < CONFIG.HEIGHT; ++row) {
            for (int dy = -CONFIG.BORDER_THICKNESS; dy <= 0; ++dy) {
                field[row][CONFIG.BORDER_THICKNESS + dy].Character = '@';
                field[row][CONFIG.BORDER_THICKNESS + dy].Color = TColorPair(0b110, 0b100);
                field[row][CONFIG.WIDTH - 1 + dy].Character = '@';
                field[row][CONFIG.WIDTH - 1 + dy].Color = TColorPair(0b110, 0b100);
            }
        }
        for (int col = 0; col < CONFIG.WIDTH; ++col) {
            for (int dx = -CONFIG.BORDER_THICKNESS; dx <= 0; ++dx) {
                field[CONFIG.BORDER_THICKNESS + dx][col].Character = '@';
                field[CONFIG.BORDER_THICKNESS + dx][col].Color = TColorPair(0b110, 0b100);
                field[CONFIG.HEIGHT - 1 + dx][col].Character = '@';
                field[CONFIG.HEIGHT - 1 + dx][col].Color = TColorPair(0b110, 0b100);
            }
        }
    }

    void AddPockets() {
    }

    TPlayingField()
        : field(TFieldMatrix(
                    CONFIG.HEIGHT,
                    vector<TFieldCell>(
                        CONFIG.WIDTH,
                        {'.', TColorPair(0b1100, 0b010)}
                    )
                ))
        , Balls()
    {
        // TODO: randomize it
        vector<int> ball_order = {1, 9, 10, 2, 8, 3, 11, 4, 12, 13, 5, 14, 6, 15, 7};
        TPosition head_position(CONFIG.WIDTH / 2, CONFIG.HEIGHT - 3 * (CONFIG.HEIGHT / 5));
        size_t idx = 0;
        Balls.emplace_back(0, TPosition(CONFIG.WIDTH / 2, 2 * CONFIG.HEIGHT / 3));
        for (size_t row_number = 0; row_number < 5; ++row_number) {
            TPosition cur_position = head_position;
            cur_position.y -= row_number * CONFIG.BALL_RADIUS * 2;
            cur_position.x -= int(CONFIG.BALL_RADIUS * row_number);
            for (size_t col_number = 0; col_number <= row_number; ++col_number) {
                auto curBallId= ball_order[idx];
                Balls.emplace_back(curBallId, cur_position);

                idx++;
                cur_position.x += 2 * CONFIG.BALL_RADIUS;
            }
        }

        // Draw the border

        // Draw pockets
        // TODO make an enum with all char mappings
        // Top left
        for (int dx = -CONFIG.POCKET_RADIUS; dx <= CONFIG.POCKET_RADIUS; ++dx) {
            for (int dy = -CONFIG.POCKET_RADIUS; dy <= CONFIG.POCKET_RADIUS; ++dy) {
                for (auto pocket_pos : pockets) {
                    int x = std::min(std::max(1, pocket_pos.x + dx), CONFIG.WIDTH - 2);
                    int y = std::min(std::max(1, pocket_pos.y + dy), CONFIG.HEIGHT - 2);
                    if (field[y][x].Character == '@') {
                        field[y][x].Character = '*';
                        field[y][x].Color = COLOR_MAP.at(EColor::Black);
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

    TFieldMatrix BuildFieldMatrix() const {
        // TODO: this is dumb, probably don't need to keep field in memory at all
        auto newField = field;
        for (const auto& ball : Balls) {
            auto ballPos = ball.GetPosition();
            // iterate over [-radius, radius) just so everything looks square and nice
            const auto& ballSprite = ball.GetBallSprite();
            for (size_t rowIdx = 0; rowIdx < ballSprite.size(); ++rowIdx) {
                for (size_t colIdx = 0; colIdx < ballSprite.size(); ++colIdx) {
                    newField[ballPos.y + rowIdx][ballPos.x + colIdx] = ballSprite[rowIdx][colIdx];
                }
            }
        }
        return newField;
    }
};
