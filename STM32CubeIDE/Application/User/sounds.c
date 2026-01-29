/**
 * @file sounds.c
 * @brief Audio system implementation for UART-based sound effect transmission.
 *
 * Implements sound effect command transmission to external audio player.
 * Commands are sent as null-terminated ASCII strings over UART1.
 *
 * @author Nguyen Viet Tuan Kiet
 * @date Created: Jan 29, 2026
 */

#include "sounds.h"
#include "stm32f4xx_hal.h"
#include <string.h>

//-----------------------------------------------------------------------------
// External Dependencies
//-----------------------------------------------------------------------------

/** @brief UART handle for audio communication (defined in main.c) */
extern UART_HandleTypeDef huart1;

//-----------------------------------------------------------------------------
// Module Configuration
//-----------------------------------------------------------------------------

/** @brief Transmission timeout in milliseconds */
#define SFX_TRANSMIT_TIMEOUT HAL_MAX_DELAY

/** @brief Enable/disable debug output for audio commands */
#define SFX_DEBUG_ENABLED 0

//-----------------------------------------------------------------------------
// Private Function Prototypes
//-----------------------------------------------------------------------------

#if SFX_DEBUG_ENABLED
static void sfx_debug_log(const char *message);
#endif

//-----------------------------------------------------------------------------
// Public API Implementation
//-----------------------------------------------------------------------------

void send_sfx_command(const char *payload)
{
    // Validate input parameter
    if (payload == NULL)
    {
        return;
    }

    // Calculate payload length including null terminator
    uint16_t length = strlen(payload) + 1;

    // Transmit command via UART (blocking)
    // Note: For non-blocking operation, consider using:
    // - HAL_UART_Transmit_IT() for interrupt-based transmission
    // - HAL_UART_Transmit_DMA() for DMA-based transmission
    HAL_StatusTypeDef status = HAL_UART_Transmit(
        &huart1,
        (uint8_t *)payload,
        length,
        SFX_TRANSMIT_TIMEOUT);

#if SFX_DEBUG_ENABLED
    if (status != HAL_OK)
    {
        sfx_debug_log("SFX transmit failed");
    }
#else
    // Suppress unused variable warning when debug disabled
    (void)status;
#endif
}

//-----------------------------------------------------------------------------
// Private Function Implementations
//-----------------------------------------------------------------------------

#if SFX_DEBUG_ENABLED
/**
 * @brief Output debug message (implementation depends on debug interface)
 * @param message Debug string to output
 */
static void sfx_debug_log(const char *message)
{
    // Implement based on available debug interface:
    // - ITM_SendChar() for SWO trace
    // - Secondary UART
    // - GPIO toggle for minimal indication
    (void)message; // Placeholder
}
#endif

//-----------------------------------------------------------------------------
// Optional Extensions (Commented - Enable if Needed)
//-----------------------------------------------------------------------------

/*
 * Non-blocking transmission implementation using interrupts.
 * Requires additional state management and callback handling.
 *
 * static volatile uint8_t sfx_tx_busy = 0;
 *
 * void send_sfx_command_async(const char *payload)
 * {
 *     if (sfx_tx_busy || payload == NULL)
 *     {
 *         return; // Drop command if busy
 *     }
 *
 *     sfx_tx_busy = 1;
 *     uint16_t length = strlen(payload) + 1;
 *     HAL_UART_Transmit_IT(&huart1, (uint8_t *)payload, length);
 * }
 *
 * // Callback to clear busy flag (add to stm32f4xx_it.c)
 * void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
 * {
 *     if (huart == &huart1)
 *     {
 *         sfx_tx_busy = 0;
 *     }
 * }
 */

/*
 * Command queue for sequential playback.
 * Useful for complex audio events.
 *
 * #define SFX_QUEUE_SIZE 4
 * static const char *sfx_queue[SFX_QUEUE_SIZE];
 * static uint8_t sfx_queue_head = 0;
 * static uint8_t sfx_queue_tail = 0;
 *
 * void queue_sfx_command(const char *payload)
 * {
 *     uint8_t next = (sfx_queue_head + 1) % SFX_QUEUE_SIZE;
 *     if (next != sfx_queue_tail)
 *     {
 *         sfx_queue[sfx_queue_head] = payload;
 *         sfx_queue_head = next;
 *     }
 * }
 *
 * void process_sfx_queue(void)
 * {
 *     if (sfx_queue_head != sfx_queue_tail && !sfx_tx_busy)
 *     {
 *         send_sfx_command_async(sfx_queue[sfx_queue_tail]);
 *         sfx_queue_tail = (sfx_queue_tail + 1) % SFX_QUEUE_SIZE;
 *     }
 * }
 */