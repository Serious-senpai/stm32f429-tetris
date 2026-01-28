#ifndef GAMEPRESENTER_HPP
#define GAMEPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class GameView;

class GamePresenter : public touchgfx::Presenter, public ModelListener
{
public:
    GamePresenter(GameView &v);

    /**
     * The activate function is called automatically when this screen is "switched in"
     * (ie. made active). Initialization logic can be placed here.
     */
    virtual void activate();

    /**
     * The deactivate function is called automatically when this screen is "switched out"
     * (ie. made inactive). Teardown functionality can be placed here.
     */
    virtual void deactivate();

    virtual ~GamePresenter() {}

    virtual void on_score_change(uint32_t new_score) override;
    virtual void on_highscore_change(uint32_t new_score) override;
    virtual void display_board(const uint8_t board[TETRIS_BOARD_WIDTH][TETRIS_BOARD_HEIGHT]) override;
    virtual void gameover() override;

private:
    GamePresenter();

    GameView &view;
};

#endif // GAMEPRESENTER_HPP
