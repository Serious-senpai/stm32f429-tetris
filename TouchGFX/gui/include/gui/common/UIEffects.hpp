#ifndef UI_EFFECTS_HPP
#define UI_EFFECTS_HPP

#include <cstdint>
#include <touchgfx/hal/Types.hpp>

/**
 * @file UIEffects.hpp
 * @brief Utility constants and functions for UI animations and visual effects.
 *
 * Provides centralized configuration for animation timings, easing functions,
 * and color manipulation utilities used across TouchGFX views.
 *
 * @author Nguyen Viet Tuan Kiet
 * @note Part of TouchGFX UI layer for STM32 Tetris project
 */
namespace UIEffects
{
    //-------------------------------------------------------------------------
    // Animation Timing Constants (in ticks, assuming 60fps refresh rate)
    //-------------------------------------------------------------------------

    /** @brief Standard fade animation duration (~500ms) */
    constexpr uint16_t FADE_DURATION_STANDARD = 30;

    /** @brief Quick fade for responsive UI feedback (~250ms) */
    constexpr uint16_t FADE_DURATION_FAST = 15;

    /** @brief Slow fade for dramatic effects (~1000ms) */
    constexpr uint16_t FADE_DURATION_SLOW = 60;

    /** @brief Line clear flash effect duration (~100ms) */
    constexpr uint16_t FLASH_DURATION = 6;

    /** @brief Button pulse cycle period (~1000ms) */
    constexpr uint16_t PULSE_CYCLE_DURATION = 60;

    //-------------------------------------------------------------------------
    // Alpha Value Constants
    //-------------------------------------------------------------------------

    /** @brief Fully opaque alpha value */
    constexpr uint8_t ALPHA_OPAQUE = 255;

    /** @brief Fully transparent alpha value */
    constexpr uint8_t ALPHA_TRANSPARENT = 0;

    /** @brief Semi-transparent for overlay effects */
    constexpr uint8_t ALPHA_OVERLAY = 180;

    /** @brief Minimum alpha during pulse animation */
    constexpr uint8_t ALPHA_PULSE_MIN = 200;

    //-------------------------------------------------------------------------
    // Easing Functions (Integer Math for Embedded)
    //-------------------------------------------------------------------------

    /**
     * @brief Linear interpolation between two values.
     *
     * @param start Starting value
     * @param end Ending value
     * @param progress Progress from 0 to maxProgress
     * @param maxProgress Maximum progress value (typically animation duration)
     * @return Interpolated value
     */
    inline uint8_t lerp(uint8_t start, uint8_t end, uint16_t progress, uint16_t maxProgress)
    {
        if (progress >= maxProgress)
        {
            return end;
        }

        int16_t delta = static_cast<int16_t>(end) - static_cast<int16_t>(start);
        return static_cast<uint8_t>(start + (delta * progress) / maxProgress);
    }

    /**
     * @brief Ease-out interpolation (fast start, slow end).
     *
     * Uses quadratic easing: y = 1 - (1-x)^2
     * Implemented with integer math for embedded efficiency.
     *
     * @param progress Progress from 0 to maxProgress
     * @param maxProgress Maximum progress value
     * @return Eased progress value (0 to maxProgress)
     */
    inline uint16_t easeOut(uint16_t progress, uint16_t maxProgress)
    {
        if (progress >= maxProgress)
        {
            return maxProgress;
        }

        // Calculate (1 - x) scaled to avoid floating point
        uint32_t inverse = maxProgress - progress;

        // Square the inverse: (1-x)^2
        uint32_t squared = (inverse * inverse) / maxProgress;

        // Return 1 - (1-x)^2
        return static_cast<uint16_t>(maxProgress - squared);
    }

    /**
     * @brief Ease-in-out interpolation (smooth start and end).
     *
     * Uses piecewise quadratic easing for smooth transitions.
     *
     * @param progress Progress from 0 to maxProgress
     * @param maxProgress Maximum progress value
     * @return Eased progress value (0 to maxProgress)
     */
    inline uint16_t easeInOut(uint16_t progress, uint16_t maxProgress)
    {
        if (progress >= maxProgress)
        {
            return maxProgress;
        }

        uint16_t halfMax = maxProgress / 2;

        if (progress < halfMax)
        {
            // First half: ease-in (accelerating)
            uint32_t scaled = (progress * progress) / halfMax;
            return static_cast<uint16_t>(scaled / 2);
        }
        else
        {
            // Second half: ease-out (decelerating)
            uint16_t adjusted = progress - halfMax;
            uint32_t inverse = halfMax - adjusted;
            uint32_t squared = (inverse * inverse) / halfMax;
            return static_cast<uint16_t>(halfMax + (halfMax - squared) / 2);
        }
    }

    //-------------------------------------------------------------------------
    // Color Utilities
    //-------------------------------------------------------------------------

    /**
     * @brief Pack RGB components into RGB565 format.
     *
     * @param r Red component (0-255)
     * @param g Green component (0-255)
     * @param b Blue component (0-255)
     * @return 16-bit RGB565 color value
     */
    inline uint16_t packRGB565(uint8_t r, uint8_t g, uint8_t b)
    {
        return static_cast<uint16_t>(
            ((r & 0xF8) << 8) |
            ((g & 0xFC) << 3) |
            ((b & 0xF8) >> 3));
    }

    /**
     * @brief Blend two RGB565 colors with specified alpha.
     *
     * @param foreground Foreground color
     * @param background Background color
     * @param alpha Blend factor (0=background, 255=foreground)
     * @return Blended RGB565 color
     */
    inline uint16_t blendRGB565(uint16_t foreground, uint16_t background, uint8_t alpha)
    {
        // Extract components
        uint8_t fR = (foreground >> 11) & 0x1F;
        uint8_t fG = (foreground >> 5) & 0x3F;
        uint8_t fB = foreground & 0x1F;

        uint8_t bR = (background >> 11) & 0x1F;
        uint8_t bG = (background >> 5) & 0x3F;
        uint8_t bB = background & 0x1F;

        // Blend components
        uint8_t rR = static_cast<uint8_t>((fR * alpha + bR * (255 - alpha)) / 255);
        uint8_t rG = static_cast<uint8_t>((fG * alpha + bG * (255 - alpha)) / 255);
        uint8_t rB = static_cast<uint8_t>((fB * alpha + bB * (255 - alpha)) / 255);

        // Pack result
        return static_cast<uint16_t>((rR << 11) | (rG << 5) | rB);
    }

} // namespace UIEffects

#endif // UI_EFFECTS_HPP