#ifndef MODEL_HPP
#define MODEL_HPP

#include <cstddef>
#include <cstdint>

#include <gui/common/Tetris.hpp>

class ModelListener;

class ActivePiece
{
private:
    inline static constexpr uint16_t _PIECES[7][4] = {
        // I
        {0xF000, 0x1111, 0xF000, 0x1111},
        // O
        {0x3300, 0x3300, 0x3300, 0x3300},
        // T
        {0x7200, 0x2620, 0x2700, 0x2320},
        // S
        {0x3600, 0x4620, 0x3600, 0x4620},
        // Z
        {0xC600, 0x2640, 0xC600, 0x2640},
        // J
        {0x1700, 0x6220, 0x7400, 0x2230},
        // L
        {0x4700, 0x2260, 0x7100, 0x3220}};

    size_t _type;
    size_t _rotation;

    /// @brief The column of the bottom-left corner of the piece.
    int8_t _column;

    /// @brief The row of the bottom-left corner of the piece.
    int8_t _row;

    bool _is_valid_move(int8_t dcolumn, int8_t drow, const uint8_t board[TETRIS_BOARD_WIDTH][TETRIS_BOARD_HEIGHT]) const;

public:
    explicit ActivePiece();

    void randomize();

    uint16_t bitmap() const
    {
        return _PIECES[_type][_rotation];
    }

    bool rotate(uint8_t board[TETRIS_BOARD_WIDTH][TETRIS_BOARD_HEIGHT]);
    bool move(int8_t dcolumn, int8_t drow, uint8_t board[TETRIS_BOARD_WIDTH][TETRIS_BOARD_HEIGHT]);

    int8_t row() const { return _row; }
    int8_t column() const { return _column; }
};

class Model
{
private:
    uint32_t _score;
    uint32_t _highscore;

    uint8_t _board[TETRIS_BOARD_WIDTH][TETRIS_BOARD_HEIGHT];

    ActivePiece _active;

    bool _fall();
    void _add_score();
    void _check_final_state();

public:
    static const uint32_t MILLISECONDS_BETWEEN_FALLS = 1000;

    bool ingame;

    Model();

    void reset();

    void bind(ModelListener *listener)
    {
        modelListener = listener;
    }

    void tick();

    void display() const;

    uint32_t get_score() const
    {
        return _score;
    }

    uint32_t get_highscore() const
    {
        return _highscore;
    }

protected:
    ModelListener *modelListener;
};

#endif // MODEL_HPP
