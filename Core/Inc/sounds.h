/**
 * @file sounds.h
 * @brief Audio system interface for UART-based sound effect transmission.
 *
 * Provides functions to send audio commands to external Python audio player
 * via UART1. Commands are transmitted as null-terminated ASCII strings.
 *
 * Hardware Configuration:
 * - Peripheral: UART1 (huart1)
 * - TX Pin: PA9 (USART1_TX)
 * - Baud Rate: 115200
 * - Protocol: Null-terminated strings
 *
 * Supported Commands (defined by Python player):
 * - "CONTROL" → SFX_PieceMoveLR.wav (piece movement)
 * - "SCORE"   → SFX_SpecialLineClearTriple.wav (line cleared)
 * - "GAMEOVER"→ SFX_GameOver.wav (game over)
 *
 * @author Nguyen Viet Tuan Kiet
 * @note Requires external Python audio player running on host computer
 */

#ifndef SOUNDS_H
#define SOUNDS_H

#ifdef __cplusplus
extern "C"
{
#endif

//-----------------------------------------------------------------------------
// Audio Command Definitions
//-----------------------------------------------------------------------------

/** @brief Sound effect for piece movement (left/right/rotate) */
#define SFX_COMMAND_CONTROL "CONTROL"

/** @brief Sound effect for line clear scoring */
#define SFX_COMMAND_SCORE "SCORE"

/** @brief Sound effect for game over event */
#define SFX_COMMAND_GAMEOVER "GAMEOVER"

//-----------------------------------------------------------------------------
// Audio Transmission Function
//-----------------------------------------------------------------------------

/**
 * @brief Transmit a sound effect command via UART to audio player.
 *
 * Sends the specified payload string followed by null terminator over
 * UART1 using blocking transmission. The external Python audio player
 * receives and decodes this command to play the corresponding WAV file.
 *
 * @param payload Null-terminated command string (use SFX_COMMAND_* defines)
 *
 * @note This function blocks until transmission completes (HAL_MAX_DELAY).
 *       For non-blocking behavior, modify to use DMA or IT transfer.
 *
 * Usage example:
 * @code
 *   send_sfx_command(SFX_COMMAND_CONTROL);  // Play movement sound
 *   send_sfx_command(SFX_COMMAND_SCORE);    // Play line clear sound
 *   send_sfx_command(SFX_COMMAND_GAMEOVER); // Play game over sound
 * @endcode
 */
void send_sfx_command(const char *payload);

//-----------------------------------------------------------------------------
// Audio System State (Optional Extension Points)
//-----------------------------------------------------------------------------

/**
 * @brief Check if audio system is ready for transmission.
 *
 * Verifies UART peripheral is initialized and ready to accept
 * new transmission requests.
 *
 * @return 1 if ready, 0 if busy or not initialized
 *
 * @note Optional utility - not required for basic operation
 */
// int is_audio_ready(void);

/**
 * @brief Queue multiple audio commands for sequential playback.
 *
 * Useful for compound events like multi-line clears (Tetris)
 * where multiple sounds should play in sequence.
 *
 * @param commands Array of command strings
 * @param count Number of commands in array
 *
 * @note Optional extension - requires queue implementation
 */
// void queue_sfx_commands(const char **commands, int count);

#ifdef __cplusplus
}
#endif

#endif // SOUNDS_H