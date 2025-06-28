/*
 * rgb_controller.c
 *
 *  Created on: Jun 28, 2025
 *      Author: UnikoZera
 */

#include "rgb_controller.h"

// ---  ---
// These values are calculated based on the timer frequency.
// Assuming the timer clock is 170MHz, PSC=0, ARR=212.
// T_period = 1 / (170Mhz / 213) = 1.253us (~800kHz)
// 1 clock tick = 1 / 170Mhz = 5.88ns
#define WS2812_HIGH_BIT 136 // High time: 0.8us -> 0.8us / 5.88ns = 136
#define WS2812_LOW_BIT  68  // High time: 0.4us -> 0.4us / 5.88ns = 68
#define RESET_PULSE_LEN 50 // Number of zero-value bytes to create the reset pulse

static uint16_t ws2812_buffer[WS2812_BUFFER_SIZE + RESET_PULSE_LEN];

void RGB_Init(void)
{
    // Initialize all LEDs to off
    for (int i = 0; i < NUM_LEDS; i++)
    {
        RGB_SetColor(i, 0, 0, 0);
    }
    // Set reset pulse buffer to 0
    for (int i = 0; i < RESET_PULSE_LEN; i++)
    {
        ws2812_buffer[WS2812_BUFFER_SIZE + i] = 0;
    }
    RGB_Update();
}

// range: 0~255
// led_index: 0 to NUM_LEDS-1
void RGB_SetColor(uint8_t led_index, uint8_t red, uint8_t green, uint8_t blue)
{
    if (led_index >= NUM_LEDS)
    {
        return;
    }

    uint32_t color = (green << 16) | (red << 8) | blue;
    uint16_t *buffer_ptr = &ws2812_buffer[led_index * BITS_PER_LED];

    for (int i = 0; i < BITS_PER_LED; i++)
    {
        if ((color << i) & 0x800000)
        { // Check MSB of the 24-bit color
            *buffer_ptr++ = WS2812_HIGH_BIT;
        }
        else
        {
            *buffer_ptr++ = WS2812_LOW_BIT;
        }
    }
}

// Send the buffer to the LEDs
void RGB_Update(void)
{
    HAL_TIM_PWM_Start_DMA(&htim3, TIM_CHANNEL_1, (uint32_t *)ws2812_buffer, sizeof(ws2812_buffer) / sizeof(uint16_t));
}