#ifndef MODELLISTENER_HPP
#define MODELLISTENER_HPP

#include <stdint.h>
#include <gui/common/Tetris.hpp>
#include <gui/model/Model.hpp>

class ModelListener
{
public:
    ModelListener() : model(0) {}

    virtual ~ModelListener() {}

    void bind(Model *m)
    {
        model = m;
        on_score_change(model->get_score());
        on_highscore_change(model->get_highscore());
    }

    virtual void on_score_change(uint32_t new_score) {}
    virtual void on_highscore_change(uint32_t new_score) {}
    virtual void display_board(const uint8_t board[TETRIS_BOARD_WIDTH][TETRIS_BOARD_HEIGHT]) {}
    virtual void gameover() {}

protected:
    Model *model;
};

#endif // MODELLISTENER_HPP
