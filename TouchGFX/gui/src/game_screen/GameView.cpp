#include <gui/game_screen/GameView.hpp>

GameView::GameView()
{
}

void GameView::setupScreen()
{
    GameViewBase::setupScreen();

    score_text.setWildcard(_score_buffer);
    on_score_change(0);

    highscore_text.setWildcard(_high_score_buffer);

    auto bitmap = tile.getBitmap();
    for (int16_t r = 0; r < TETRIS_BOARD_HEIGHT; r++)
    {
        for (int16_t c = 0; c < TETRIS_BOARD_WIDTH; c++)
        {
            int16_t x = TETRIS_FIELD_LEFT + c * TETRIS_TILE_SIZE;
            int16_t y = TETRIS_FIELD_BOTTOM - (r + 1) * TETRIS_TILE_SIZE;

            auto &tile = _board[c][r] = touchgfx::Image(bitmap);
            tile.setXY(x, y);
            tile.setVisible(false);

            remove(tile);
            add(tile);
        }
    }
}

void GameView::tearDownScreen()
{
    GameViewBase::tearDownScreen();
}

void GameView::on_score_change(uint32_t new_score)
{
    touchgfx::Unicode::snprintf(
        _score_buffer,
        _SCORE_BUFFER_SIZE,
        "%u",
        new_score);

    score_text.invalidate();
}

void GameView::on_highscore_change(uint32_t new_score)
{
    touchgfx::Unicode::snprintf(
        _high_score_buffer,
        _SCORE_BUFFER_SIZE,
        "%u",
        new_score);

    highscore_text.invalidate();
}

void GameView::display_board(const uint8_t board[TETRIS_BOARD_WIDTH][TETRIS_BOARD_HEIGHT])
{
    for (size_t r = 0; r < TETRIS_BOARD_HEIGHT; r++)
    {
        for (size_t c = 0; c < TETRIS_BOARD_WIDTH; c++)
        {
            auto &tile = _board[c][r];
            if (board[c][r] != 0)
            {
                tile.setVisible(true);
            }
            else
            {
                tile.setVisible(false);
            }

            tile.invalidate();
        }
    }
}

void GameView::set_gameover()
{
    // Add gameover image to the top
    remove(gameover);
    add(gameover);
    gameover.setXY(86, 96);
    gameover.invalidate();
}
