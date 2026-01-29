#include <cmsis_os.h>
#include <messages.h>
#include <sounds.h>
#include <stm32f4xx_hal.h>
#include <string.h>
#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>

extern osMessageQueueId_t channel;
extern RNG_HandleTypeDef hrng;

bool ActivePiece::_is_valid_move(int8_t dcolumn, int8_t drow, const uint8_t board[TETRIS_BOARD_WIDTH][TETRIS_BOARD_HEIGHT]) const
{
    auto new_column = _column + dcolumn;
    auto new_row = _row + drow;
    auto bitmap = this->bitmap();

    bool available[TETRIS_BOARD_WIDTH][TETRIS_BOARD_HEIGHT] = {};
    for (size_t r = 0; r < 4; r++)
    {
        for (size_t c = 0; c < 4; c++)
        {
            if ((bitmap & (uint16_t(1) << ((r << 2) + c))) != 0)
            {
                available[_column + c][_row + r] = true;
            }
        }
    }

    for (size_t r = 0; r < 4; r++)
    {
        for (size_t c = 0; c < 4; c++)
        {
            if ((bitmap & (uint16_t(1) << ((r << 2) + c))) != 0)
            {
                auto column = new_column + c;
                auto row = new_row + r;
                if (column < 0 || column >= TETRIS_BOARD_WIDTH || row < 0 || row >= TETRIS_BOARD_HEIGHT)
                {
                    return false;
                }

                if (!available[column][row] && board[column][row] != 0)
                {
                    return false;
                }
            }
        }
    }

    return true;
}

ActivePiece::ActivePiece()
    : _type(0),
      _rotation(0),
      _column(TETRIS_BOARD_WIDTH / 2 - 2),
      _row(TETRIS_BOARD_HEIGHT - 5) {}

void ActivePiece::randomize()
{
    uint32_t random = 0;
    HAL_RNG_GenerateRandomNumber(&hrng, &random);

    _type = random % 7;
    _rotation = 0;
}

bool ActivePiece::rotate(uint8_t board[TETRIS_BOARD_WIDTH][TETRIS_BOARD_HEIGHT])
{
    auto old_bitmap = this->bitmap();

    bool available[TETRIS_BOARD_WIDTH][TETRIS_BOARD_HEIGHT] = {};
    for (size_t r = 0; r < 4; r++)
    {
        for (size_t c = 0; c < 4; c++)
        {
            if ((old_bitmap & (uint16_t(1) << ((r << 2) + c))) != 0)
            {
                available[_column + c][_row + r] = true;
            }
        }
    }

    auto new_rotation = (_rotation + 1) % 4;
    auto new_bitmap = _PIECES[_type][new_rotation];

    for (size_t r = 0; r < 4; r++)
    {
        for (size_t c = 0; c < 4; c++)
        {
            if ((new_bitmap & (uint16_t(1) << ((r << 2) + c))) != 0)
            {
                auto column = _column + c;
                auto row = _row + r;
                if (column < 0 || column >= TETRIS_BOARD_WIDTH || row < 0 || row >= TETRIS_BOARD_HEIGHT)
                {
                    return false;
                }

                if (!available[column][row] && board[column][row] != 0)
                {
                    return false;
                }
            }
        }
    }

    for (size_t r = 0; r < 4; r++)
    {
        for (size_t c = 0; c < 4; c++)
        {
            if ((old_bitmap & (uint16_t(1) << ((r << 2) + c))) != 0)
            {
                board[_column + c][_row + r] = 0;
            }
        }
    }

    for (size_t r = 0; r < 4; r++)
    {
        for (size_t c = 0; c < 4; c++)
        {
            if ((new_bitmap & (uint16_t(1) << ((r << 2) + c))) != 0)
            {
                auto column = _column + c;
                auto row = _row + r;
                board[column][row] = ~0;
            }
        }
    }

    _rotation = new_rotation;
    return true;
}

bool ActivePiece::move(int8_t dcolumn, int8_t drow, uint8_t board[TETRIS_BOARD_WIDTH][TETRIS_BOARD_HEIGHT])
{
    if (_is_valid_move(dcolumn, drow, board))
    {
        // A more efficient would be to update based on the difference area. But here the block is small enough.
        auto mask = bitmap();
        for (size_t r = 0; r < 4; r++)
        {
            for (size_t c = 0; c < 4; c++)
            {
                if ((mask & (uint16_t(1) << ((r << 2) + c))) != 0)
                {
                    board[_column + c][_row + r] = 0;
                }
            }
        }

        _column += dcolumn;
        _row += drow;

        for (size_t r = 0; r < 4; r++)
        {
            for (size_t c = 0; c < 4; c++)
            {
                if ((mask & (uint16_t(1) << ((r << 2) + c))) != 0)
                {
                    board[_column + c][_row + r] = ~0;
                }
            }
        }

        return true;
    }

    return false;
}

bool Model::_fall()
{
    return _active.move(0, -1, _board);
}

void Model::_add_score()
{
    send_sfx_command("SCORE");
    modelListener->on_score_change(++_score);
    if (_score > _highscore)
    {
        _highscore = _score;
        modelListener->on_highscore_change(_highscore);
    }
}

void Model::_check_final_state()
{
    for (int16_t r = 0; r < TETRIS_BOARD_HEIGHT; r++)
    {
        bool full = true;
        for (int16_t c = 0; c < TETRIS_BOARD_WIDTH; c++)
        {
            if (_board[c][r] == 0)
            {
                full = false;
                break;
            }
        }

        if (full)
        {
            _add_score();
            for (int16_t rr = r; rr < TETRIS_BOARD_HEIGHT - 1; rr++)
            {
                for (int16_t c = 0; c < TETRIS_BOARD_WIDTH; c++)
                {
                    _board[c][rr] = _board[c][rr + 1];
                }
            }

            for (int16_t c = 0; c < TETRIS_BOARD_WIDTH; c++)
            {
                _board[c][TETRIS_BOARD_HEIGHT - 1] = 0;
            }

            r--;
        }
    }

    bool gameover = false;
    for (size_t c = 0; c < TETRIS_BOARD_WIDTH; c++)
    {
        if (_board[c][TETRIS_GAMEOVER_ROW] != 0)
        {
            gameover = true;
            break;
        }
    }

    if (gameover)
    {
        modelListener->gameover();
        send_sfx_command("GAMEOVER");
        ingame = false;
    }

    _active = ActivePiece();
    _active.randomize();
}

Model::Model() : _highscore(0), _active(), ingame(false), modelListener(nullptr)
{
    reset();
}

void Model::reset()
{
    _score = 0;
    _active.randomize();
    memset(_board, 0, sizeof(_board));

    auto bitmap = _active.bitmap();
    auto column = _active.column();
    auto row = _active.row();
    for (size_t r = 0; r < 4; r++)
    {
        for (size_t c = 0; c < 4; c++)
        {
            if ((bitmap & (uint16_t(1) << ((r << 2) + c))) != 0)
            {
                _board[column + c][row + r] = ~0;
            }
        }
    }
}

void Model::tick()
{
    if (!ingame)
    {
        return;
    }

    MessageType message;
    while (modelListener != nullptr && osMessageQueueGet(channel, &message, NULL, 0) == osOK)
    {
        if (message == MESSAGE_BUTTON_UP)
        {
            _active.rotate(_board);
        }
        else if (message == MESSAGE_BUTTON_RIGHT)
        {
            _active.move(1, 0, _board);
        }
        else if (message == MESSAGE_BUTTON_DOWN)
        {
            while (_fall())
            {
            }
        }
        else if (message == MESSAGE_BUTTON_LEFT)
        {
            _active.move(-1, 0, _board);
        }

        send_sfx_command("CONTROL");
        display();
    }

    static const uint32_t ticks_to_wait = osKernelGetTickFreq() * MILLISECONDS_BETWEEN_FALLS / 1000;
    static uint32_t last = 0;

    uint32_t current = osKernelGetTickCount();
    if (current - last >= ticks_to_wait)
    {
        last = current;
        if (!_fall())
        {
            _check_final_state();
        }

        display();
    }
}

void Model::display() const
{
    modelListener->display_board(_board);
}
