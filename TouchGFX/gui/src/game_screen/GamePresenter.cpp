#include <gui/game_screen/GameView.hpp>
#include <gui/game_screen/GamePresenter.hpp>

GamePresenter::GamePresenter(GameView& v)
    : view(v)
{
}

void GamePresenter::activate()
{
    // Signal model that game screen is active
    // This enables tick() processing in FrontendApplication::handleTickEvent()
    if (model != nullptr)
    {
        // Reset game state for fresh start
        model->reset();
        model->ingame = true;

        // Initialize view with current state
        view.on_score_change(model->get_score());
        view.on_highscore_change(model->get_highscore());
    }
}

void GamePresenter::deactivate()
{
    // Signal model that game screen is inactive
    // Prevents tick() from processing game logic while on other screens
    if (model != nullptr)
    {
        model->ingame = false;
    }
}

void GamePresenter::on_score_change(uint32_t new_score)
{
    // Forward score update to view for display
    // Audio feedback (SCORE sound) is handled by Model._add_score()
    view.on_score_change(new_score);
}

void GamePresenter::on_highscore_change(uint32_t new_score)
{
    // Forward high score update to view
    // This persists across game sessions within same power cycle
    view.on_highscore_change(new_score);
}

void GamePresenter::display_board(const uint8_t board[TETRIS_BOARD_WIDTH][TETRIS_BOARD_HEIGHT])
{
    // Forward complete board state to view for rendering
    // View handles tile visibility and line clear animations
    view.display_board(board);
}

void GamePresenter::gameover()
{
    // Forward game over event to view
    // Triggers overlay display and disables input processing
    // Audio "GAMEOVER" command is sent directly by Model._check_final_state()
    view.set_gameover();
}

bool GamePresenter::isGameActive() const
{
    if (model != nullptr)
    {
        return model->ingame;
    }
    return false;
}

void GamePresenter::requestNewGame()
{
    if (model != nullptr)
    {
        model->reset();
        model->ingame = true;

        // Refresh view with reset state
        view.on_score_change(model->get_score());
        model->display();
    }
}