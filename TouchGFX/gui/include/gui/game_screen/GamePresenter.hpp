#ifndef GAMEPRESENTER_HPP
#define GAMEPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class GameView;

/**
 * @class GamePresenter
 * @brief Presenter for the Game screen, implementing MVP pattern bridge.
 *
 * Acts as the intermediary between Model (game logic) and GameView (UI),
 * handling event forwarding and state synchronization. Implements
 * ModelListener interface to receive game state change notifications.
 *
 * Event Flow:
 * - Model.tick() → detects state changes → calls ModelListener methods
 * - Presenter receives calls → forwards to appropriate View methods
 * - View updates visual representation → triggers screen refresh
 *
 * @author Nguyen Viet Tuan Kiet
 * @note Audio integration handled via sounds.c UART transmission
 */
class GamePresenter : public touchgfx::Presenter, public ModelListener
{
public:
    GamePresenter(GameView& v);

    /**
     * @brief Called when game screen becomes active.
     *
     * Sets model to ingame state, triggering game tick processing.
     * Resets game state if entering from welcome screen.
     */
    virtual void activate();

    /**
     * @brief Called when leaving game screen.
     *
     * Sets model to not-ingame state, pausing game logic processing.
     */
    virtual void deactivate();

    virtual ~GamePresenter() {}

    /**
     * @brief Handle score increment event from model.
     *
     * Triggered when player clears a line. Forwards to view for
     * score display update.
     *
     * @param new_score Updated cumulative score
     */
    virtual void on_score_change(uint32_t new_score) override;

    /**
     * @brief Handle high score update event from model.
     *
     * Triggered when current score exceeds previous high score.
     * Forwards to view for persistent display update.
     *
     * @param new_score New high score value
     */
    virtual void on_highscore_change(uint32_t new_score) override;

    /**
     * @brief Handle board state change event from model.
     *
     * Called every time piece moves, rotates, or locks. Forwards
     * complete board state to view for re-rendering.
     *
     * @param board 2D array of cell occupancy (0 = empty, non-zero = filled)
     */
    virtual void display_board(const uint8_t board[TETRIS_BOARD_WIDTH][TETRIS_BOARD_HEIGHT]) override;

    /**
     * @brief Handle game over event from model.
     *
     * Triggered when a block occupies TETRIS_GAMEOVER_ROW.
     * Forwards to view for game over overlay display.
     * Note: Audio "GAMEOVER" command sent by Model directly.
     */
    virtual void gameover() override;

    /**
     * @brief Check if game is currently in progress.
     * @return true if model is in ingame state
     */
    bool isGameActive() const;

    /**
     * @brief Request model to reset for new game.
     *
     * Clears board, resets score, spawns new piece.
     */
    void requestNewGame();

private:
    GamePresenter();

    GameView& view;
};

#endif // GAMEPRESENTER_HPP