#include <gui/game_screen/GameView.hpp>
#include <gui/game_screen/GamePresenter.hpp>

GamePresenter::GamePresenter(GameView &v)
    : view(v)
{
}

void GamePresenter::activate()
{
    model->ingame = true;
}

void GamePresenter::deactivate()
{
    model->ingame = false;
}

void GamePresenter::on_score_change(uint32_t new_score)
{
    view.on_score_change(new_score);
}

void GamePresenter::on_highscore_change(uint32_t new_score)
{
    view.on_highscore_change(new_score);
}

void GamePresenter::display_board(const uint8_t board[TETRIS_BOARD_WIDTH][TETRIS_BOARD_HEIGHT])
{
    view.display_board(board);
}

void GamePresenter::gameover()
{
    view.set_gameover();
}
