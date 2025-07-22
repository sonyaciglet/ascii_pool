#include "field.h"


    TPlayingField::TBall::EBallType TPlayingField::TBall::GetBallType() const {
        if (Id_ == CUE_BALL_ID) {
            return EBallType::CueBall;
        } else if (Id_ >= 1 && Id_ < 9) {
            return EBallType::Solid;
        } else if (Id_ >= 9 && Id_ < 16) {
            return EBallType::Striped;
        } else {
            std::unreachable();
        }
    }

    EColor TPlayingField::TBall::GetBallColor() const {
        if (Id_ == CUE_BALL_ID) {
            return EColor::White;
        }
        auto color_id = (Id_ - 1) % 8;
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

    void TPlayingField::TBall::DrawBall() {
        const auto ball_color = COLOR_MAP.at(GetBallColor());
        for (auto& row : BallSprite_) {
            for (auto& elem : row) {
                elem = TFieldCell('0', ball_color);
            }
        }
        if (GetBallType() == EBallType::Striped) {
            for (int i = 0; i < BallSprite_.size(); ++i) {
                BallSprite_[i][BallSprite_.size() - i - 1].Color = COLOR_MAP.at(EColor::White);
            }
        }
    }
