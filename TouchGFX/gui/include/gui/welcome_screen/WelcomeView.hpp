#ifndef WELCOMEVIEW_HPP
#define WELCOMEVIEW_HPP

#include <gui_generated/welcome_screen/WelcomeViewBase.hpp>
#include <gui/welcome_screen/WelcomePresenter.hpp>
#include <touchgfx/mixins/FadeAnimator.hpp>

/**
 * @class WelcomeView
 * @brief Welcome screen view handling UI presentation and animations.
 *
 * This view manages the welcome screen display including fade-in animations
 * for visual polish and enhanced user experience on game startup.
 *
 * @author Nguyen Viet Tuan Kiet
 * @note Part of TouchGFX UI layer implementation
 */
class WelcomeView : public WelcomeViewBase
{
public:
    WelcomeView();
    virtual ~WelcomeView() {}

    /**
     * @brief Initialize screen components and start entrance animations.
     */
    virtual void setupScreen();

    /**
     * @brief Cleanup resources when leaving this screen.
     */
    virtual void tearDownScreen();

    /**
     * @brief Called every frame tick for animation updates.
     */
    virtual void handleTickEvent();

protected:
    /**
     * @brief Initialize fade-in animation for welcome elements.
     */
    void initializeEntranceAnimation();

    /**
     * @brief Update pulsing effect on start button for visual feedback.
     */
    void updateButtonPulseEffect();

private:
    static const uint16_t FADE_IN_DURATION_TICKS = 30;    ///< ~500ms at 60fps
    static const uint16_t PULSE_CYCLE_TICKS = 60;         ///< Full pulse cycle duration
    static const uint8_t PULSE_MIN_ALPHA = 200;           ///< Minimum alpha during pulse
    static const uint8_t PULSE_MAX_ALPHA = 255;           ///< Maximum alpha during pulse

    uint16_t animationTickCounter;    ///< Counter for tracking animation progress
    uint16_t pulseTickCounter;        ///< Counter for button pulse effect
    bool entranceAnimationComplete;   ///< Flag indicating entrance animation status
};

#endif // WELCOMEVIEW_HPP