#ifndef GAMEVIEW_HPP
#define GAMEVIEW_HPP

#include <gui_generated/game_screen/GameViewBase.hpp>
#include <gui/common/Tetris.hpp>
#include <gui/game_screen/GamePresenter.hpp>

class GameView : public GameViewBase
{
private:
    static const uint16_t _SCORE_BUFFER_SIZE = 5;
    touchgfx::Unicode::UnicodeChar _score_buffer[5];
    touchgfx::Unicode::UnicodeChar _high_score_buffer[5];

    touchgfx::Image _board[TETRIS_BOARD_WIDTH][TETRIS_BOARD_HEIGHT];

public:
    GameView();
    virtual ~GameView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    void on_score_change(uint32_t new_score);
    void on_highscore_change(uint32_t new_score);
    void display_board(const uint8_t board[TETRIS_BOARD_WIDTH][TETRIS_BOARD_HEIGHT]);
    void set_gameover();

protected:
};

#endif // GAMEVIEW_HPP
