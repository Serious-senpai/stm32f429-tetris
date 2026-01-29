#include <gui/welcome_screen/WelcomeView.hpp>
#include <gui/welcome_screen/WelcomePresenter.hpp>

WelcomePresenter::WelcomePresenter(WelcomeView& v)
    : view(v)
{
}

void WelcomePresenter::activate()
{
    // Ensure game state is not active during welcome screen
    // This handles cases where user returns from game screen
    if (model != nullptr)
    {
        model->ingame = false;
    }

    // Retrieve and display current high score on welcome screen
    // This provides continuity between game sessions
    uint32_t highScore = getCurrentHighScore();
    if (highScore > 0)
    {
        // High score available from previous session
        // View can display this to motivate player
        on_highscore_change(highScore);
    }
}

void WelcomePresenter::deactivate()
{
    // Prepare for transition - no specific cleanup needed
    // Model reset will be handled by Game screen activation
}

void WelcomePresenter::on_highscore_change(uint32_t new_score)
{
    // Forward high score updates to view for display
    // This allows welcome screen to show "Best: XXX" message
    // Implementation depends on view having appropriate widgets

    // Note: View method would be called here if widget exists:
    // view.displayHighScore(new_score);

    (void)new_score; // Suppress unused warning until view method implemented
}

uint32_t WelcomePresenter::getCurrentHighScore() const
{
    if (model != nullptr)
    {
        return model->get_highscore();
    }
    return 0;
}

void WelcomePresenter::prepareNewGame()
{
    // Reset model state for fresh game start
    if (model != nullptr)
    {
        model->reset();
    }
}