#ifndef GAMEVIEW_HPP
#define GAMEVIEW_HPP

#include <gui_generated/game_screen/GameViewBase.hpp>
#include <gui/common/Tetris.hpp>
#include <gui/game_screen/GamePresenter.hpp>

/**
 * @class GameView
 * @brief Main game screen view handling Tetris board rendering and visual effects.
 *
 * Manages the visual representation of the Tetris game including:
 * - Dynamic tile rendering based on board state
 * - Score and high score text display
 * - Game over overlay presentation
 * - Visual feedback animations for line clears
 *
 * @author Nguyen Viet Tuan Kiet
 * @note Part of TouchGFX MVP pattern implementation
 */
class GameView : public GameViewBase
{
private:
    static const uint16_t SCORE_BUFFER_SIZE = 5;    ///< Buffer size for score digits
    static const uint16_t FLASH_DURATION_TICKS = 6; ///< Duration of line clear flash effect

    touchgfx::Unicode::UnicodeChar _score_buffer[SCORE_BUFFER_SIZE];
    touchgfx::Unicode::UnicodeChar _high_score_buffer[SCORE_BUFFER_SIZE];

    touchgfx::Image _board[TETRIS_BOARD_WIDTH][TETRIS_BOARD_HEIGHT];

    // Visual effect state tracking
    uint8_t _previousBoard[TETRIS_BOARD_WIDTH][TETRIS_BOARD_HEIGHT];
    uint16_t _lineFlashCounter;   ///< Counter for line clear flash animation
    bool _isFlashActive;          ///< Flag indicating flash effect is in progress
    int16_t _flashingRow;         ///< Row currently being animated (-1 if none)

public:
    GameView();
    virtual ~GameView() {}

    /**
     * @brief Initialize game screen widgets and tile grid.
     */
    virtual void setupScreen();

    /**
     * @brief Cleanup when leaving game screen.
     */
    virtual void tearDownScreen();

    /**
     * @brief Called every frame for animation updates.
     */
    virtual void handleTickEvent();

    /**
     * @brief Update score display with new value.
     * @param new_score Current score to display
     */
    void on_score_change(uint32_t new_score);

    /**
     * @brief Update high score display with new value.
     * @param new_score Current high score to display
     */
    void on_highscore_change(uint32_t new_score);

    /**
     * @brief Render the current board state to screen.
     * @param board 2D array representing occupied cells
     */
    void display_board(const uint8_t board[TETRIS_BOARD_WIDTH][TETRIS_BOARD_HEIGHT]);

    /**
     * @brief Display game over overlay with animation.
     */
    void set_gameover();

protected:
    /**
     * @brief Detect and trigger flash effect for cleared lines.
     * @param board Current board state for comparison
     */
    void detectLineClear(const uint8_t board[TETRIS_BOARD_WIDTH][TETRIS_BOARD_HEIGHT]);

    /**
     * @brief Update ongoing flash animation state.
     */
    void updateFlashEffect();

    /**
     * @brief Store current board state for next frame comparison.
     * @param board Current board state to cache
     */
    void cacheBoardState(const uint8_t board[TETRIS_BOARD_WIDTH][TETRIS_BOARD_HEIGHT]);
};

#endif // GAMEVIEW_HPP