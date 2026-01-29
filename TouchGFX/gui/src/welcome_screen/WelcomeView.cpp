#include <gui/welcome_screen/WelcomeView.hpp>

WelcomeView::WelcomeView()
    : animationTickCounter(0),
      pulseTickCounter(0),
      entranceAnimationComplete(false)
{
}

void WelcomeView::setupScreen()
{
    WelcomeViewBase::setupScreen();

    // Initialize entrance animation state
    initializeEntranceAnimation();
}

void WelcomeView::tearDownScreen()
{
    // Reset animation state for potential re-entry
    animationTickCounter = 0;
    pulseTickCounter = 0;
    entranceAnimationComplete = false;

    WelcomeViewBase::tearDownScreen();
}

void WelcomeView::handleTickEvent()
{
    // Process entrance fade-in animation
    if (!entranceAnimationComplete)
    {
        animationTickCounter++;

        if (animationTickCounter >= FADE_IN_DURATION_TICKS)
        {
            entranceAnimationComplete = true;
        }
    }
    else
    {
        // Run continuous pulse effect on interactive elements
        updateButtonPulseEffect();
    }
}

void WelcomeView::initializeEntranceAnimation()
{
    animationTickCounter = 0;
    entranceAnimationComplete = false;

    // Note: Actual fade implementation depends on TouchGFX Designer widget setup
    // This provides the framework for FadeAnimator<Image> or similar mixins
    // configured in the .touchgfx project file
}

void WelcomeView::updateButtonPulseEffect()
{
    pulseTickCounter++;

    if (pulseTickCounter >= PULSE_CYCLE_TICKS)
    {
        pulseTickCounter = 0;
    }

    // Calculate alpha using sine-wave approximation for smooth pulsing
    // Using integer math to avoid floating point on embedded target
    uint16_t halfCycle = PULSE_CYCLE_TICKS / 2;
    uint16_t position = (pulseTickCounter < halfCycle)
                            ? pulseTickCounter
                            : PULSE_CYCLE_TICKS - pulseTickCounter;

    // Linear interpolation between min and max alpha
    uint8_t alphaRange = PULSE_MAX_ALPHA - PULSE_MIN_ALPHA;
    uint8_t currentAlpha = PULSE_MIN_ALPHA +
                           (uint8_t)((position * alphaRange) / halfCycle);

    // Note: Apply to button widget if FadeAnimator mixin is enabled
    // startButton.setAlpha(currentAlpha);
    // startButton.invalidate();

    (void)currentAlpha; // Suppress unused warning until widget is connected
}