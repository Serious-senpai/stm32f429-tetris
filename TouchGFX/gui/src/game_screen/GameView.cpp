#include <gui/game_screen/GameView.hpp>
#include <cstring>

GameView::GameView()
    : _lineFlashCounter(0),
      _isFlashActive(false),
      _flashingRow(-1)
{
    // Initialize previous board state to all zeros
    memset(_previousBoard, 0, sizeof(_previousBoard));
}

void GameView::setupScreen()
{
    GameViewBase::setupScreen();

    // Initialize score display buffers
    score_text.setWildcard(_score_buffer);
    on_score_change(0);

    highscore_text.setWildcard(_high_score_buffer);

    // Build the tile grid for game board visualization
    auto bitmap = tile.getBitmap();
    for (int16_t r = 0; r < TETRIS_BOARD_HEIGHT; r++)
    {
        for (int16_t c = 0; c < TETRIS_BOARD_WIDTH; c++)
        {
            int16_t x = TETRIS_FIELD_LEFT + c * TETRIS_TILE_SIZE;
            int16_t y = TETRIS_FIELD_BOTTOM - (r + 1) * TETRIS_TILE_SIZE;

            auto& tileRef = _board[c][r] = touchgfx::Image(bitmap);
            tileRef.setXY(x, y);
            tileRef.setVisible(false);

            // Ensure proper z-ordering
            remove(tileRef);
            add(tileRef);
        }
    }

    // Reset visual effect state
    _lineFlashCounter = 0;
    _isFlashActive = false;
    _flashingRow = -1;
    memset(_previousBoard, 0, sizeof(_previousBoard));
}

void GameView::tearDownScreen()
{
    GameViewBase::tearDownScreen();
}

void GameView::handleTickEvent()
{
    // Process ongoing flash animation
    if (_isFlashActive)
    {
        updateFlashEffect();
    }
}

void GameView::on_score_change(uint32_t new_score)
{
    touchgfx::Unicode::snprintf(
        _score_buffer,
        SCORE_BUFFER_SIZE,
        "%u",
        new_score);

    score_text.invalidate();
}

void GameView::on_highscore_change(uint32_t new_score)
{
    touchgfx::Unicode::snprintf(
        _high_score_buffer,
        SCORE_BUFFER_SIZE,
        "%u",
        new_score);

    highscore_text.invalidate();
}

void GameView::display_board(const uint8_t board[TETRIS_BOARD_WIDTH][TETRIS_BOARD_HEIGHT])
{
    // Check for line clear before updating display
    detectLineClear(board);

    // Update tile visibility based on board state
    for (size_t r = 0; r < TETRIS_BOARD_HEIGHT; r++)
    {
        for (size_t c = 0; c < TETRIS_BOARD_WIDTH; c++)
        {
            auto& tileRef = _board[c][r];

            bool shouldBeVisible = (board[c][r] != 0);

            // Apply flash effect override for animating row
            if (_isFlashActive && static_cast<int16_t>(r) == _flashingRow)
            {
                // Toggle visibility for flash effect
                shouldBeVisible = (_lineFlashCounter % 4 < 2);
            }

            tileRef.setVisible(shouldBeVisible);
            tileRef.invalidate();
        }
    }

    // Cache board state for next comparison
    cacheBoardState(board);
}

void GameView::set_gameover()
{
    // Stop any ongoing effects
    _isFlashActive = false;

    // Position and display game over overlay
    remove(gameover);
    add(gameover);
    gameover.setXY(86, 96);
    gameover.invalidate();
}

void GameView::detectLineClear(const uint8_t board[TETRIS_BOARD_WIDTH][TETRIS_BOARD_HEIGHT])
{
    // Skip detection if flash already active
    if (_isFlashActive)
    {
        return;
    }

    // Check for rows that were full in previous state but shifted in new state
    // This indicates a line was just cleared
    for (int16_t r = 0; r < TETRIS_BOARD_HEIGHT - 1; r++)
    {
        bool wasFull = true;
        bool isDifferent = false;

        for (int16_t c = 0; c < TETRIS_BOARD_WIDTH; c++)
        {
            if (_previousBoard[c][r] == 0)
            {
                wasFull = false;
                break;
            }
            if (_previousBoard[c][r] != board[c][r])
            {
                isDifferent = true;
            }
        }

        // Line was cleared - trigger visual feedback
        if (wasFull && isDifferent)
        {
            _isFlashActive = true;
            _lineFlashCounter = 0;
            _flashingRow = r;
            break; // Handle one row at a time
        }
    }
}

void GameView::updateFlashEffect()
{
    _lineFlashCounter++;

    if (_lineFlashCounter >= FLASH_DURATION_TICKS)
    {
        // Flash complete - reset state
        _isFlashActive = false;
        _lineFlashCounter = 0;
        _flashingRow = -1;
    }
}

void GameView::cacheBoardState(const uint8_t board[TETRIS_BOARD_WIDTH][TETRIS_BOARD_HEIGHT])
{
    for (size_t r = 0; r < TETRIS_BOARD_HEIGHT; r++)
    {
        for (size_t c = 0; c < TETRIS_BOARD_WIDTH; c++)
        {
            _previousBoard[c][r] = board[c][r];
        }
    }
}