/*
 * sounds.c
 *
 *  Created on: Jan 29, 2026
 *      Author: Serious-senpai
 */

#include "sounds.h"
#include "stm32f4xx_hal.h"
#include "string.h"

extern UART_HandleTypeDef huart1;

void send_sfx_command(const char *payload)
{
    HAL_UART_Transmit(&huart1, (uint8_t *)payload, strlen(payload) + 1, HAL_MAX_DELAY);
}
