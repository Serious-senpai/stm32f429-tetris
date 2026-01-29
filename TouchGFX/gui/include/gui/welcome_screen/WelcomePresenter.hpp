#ifndef WELCOMEPRESENTER_HPP
#define WELCOMEPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class WelcomeView;

/**
 * @class WelcomePresenter
 * @brief Presenter for the Welcome screen, bridging Model and WelcomeView.
 *
 * Handles communication between the welcome screen UI and the game model,
 * including displaying persistent high score from previous sessions and
 * coordinating screen transitions.
 *
 * @author Nguyen Viet Tuan Kiet
 * @note Implements ModelListener interface for score event handling
 */
class WelcomePresenter : public touchgfx::Presenter, public ModelListener
{
public:
    WelcomePresenter(WelcomeView& v);

    /**
     * @brief Called when screen becomes active.
     *
     * Initializes the welcome screen state and retrieves current high score
     * from the model for display purposes.
     */
    virtual void activate();

    /**
     * @brief Called when screen is deactivated.
     *
     * Performs cleanup and ensures model state is consistent
     * before transitioning to game screen.
     */
    virtual void deactivate();

    virtual ~WelcomePresenter() {}

    /**
     * @brief Callback for high score changes.
     *
     * Updates the welcome screen display when high score changes
     * (inherited from ModelListener).
     *
     * @param new_score The updated high score value
     */
    virtual void on_highscore_change(uint32_t new_score) override;

    /**
     * @brief Request the current high score for display.
     *
     * @return Current high score from the model
     */
    uint32_t getCurrentHighScore() const;

    /**
     * @brief Prepare the model for a new game session.
     *
     * Called before transitioning to game screen to ensure
     * model is properly reset for new gameplay.
     */
    void prepareNewGame();

private:
    WelcomePresenter();

    WelcomeView& view;
};

#endif // WELCOMEPRESENTER_HPP