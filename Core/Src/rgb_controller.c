/*
 * rgb_controller.c
 *
 *  Created on: Jun 28, 2025
 *      Author: UnikoZera
 */

#include "rgb_controller.h"

// The timer clock is 170MHz, PSC=0, ARR=212.
// T_period = 1 / (170Mhz / 213) = 1.253us (~800kHz)
// Each bit is 5.88ns, so we can use a 16-bit timer to generate the PWM signal.
#define WS2812_HIGH_BIT 136 // High time: 0.8us -> 0.8us / 5.88ns = 136
#define WS2812_LOW_BIT 68   // High time: 0.4us -> 0.4us / 5.88ns = 68
#define RESET_PULSE_LEN 42  // Number of zero-value bytes to create the reset pulse (>50us) 42*1.253us = 52.626us

#define PWM_BUFFER_LEN (NUM_LEDS * 24 + RESET_PULSE_LEN) // GRB order, both need 24 bits
static uint8_t led_colors[NUM_LEDS][3];
static uint16_t pwm_buffer[PWM_BUFFER_LEN];

void RGB_Init(void)
{
    RGB_Clear();
    RGB_Update();
}

// RGB范围是0-255
// led_index: 0-3
void RGB_SetColor(uint8_t led_index, uint8_t red, uint8_t green, uint8_t blue)
{
    if (led_index < NUM_LEDS)
    {
        led_colors[led_index][0] = red;
        led_colors[led_index][1] = green;
        led_colors[led_index][2] = blue;
    }
}

void RGB_Clear(void)
{
    for (int i = 0; i < NUM_LEDS; i++)
    {
        RGB_SetColor(i, 0, 0, 0);
    }
}

//! setColor之后请直接调用这个来更新LED颜色
void RGB_Update(void)
{
    uint32_t buffer_index = 0;
    for (int i = 0; i < NUM_LEDS; i++) //? GBR的神奇写法
    {
        // Green
        for (int j = 7; j >= 0; j--)
        {
            pwm_buffer[buffer_index++] = ((led_colors[i][1] >> j) & 1) ? WS2812_HIGH_BIT : WS2812_LOW_BIT;
        }
        // Red
        for (int j = 7; j >= 0; j--)
        {
            pwm_buffer[buffer_index++] = ((led_colors[i][0] >> j) & 1) ? WS2812_HIGH_BIT : WS2812_LOW_BIT;
        }
        // Blue
        for (int j = 7; j >= 0; j--)
        {
            pwm_buffer[buffer_index++] = ((led_colors[i][2] >> j) & 1) ? WS2812_HIGH_BIT : WS2812_LOW_BIT;
        }
    }

    // Reset pulse
    for (int i = 0; i < RESET_PULSE_LEN; i++)
    {
        pwm_buffer[buffer_index++] = 0;
    }

    HAL_TIM_PWM_Start_DMA(&htim3, TIM_CHANNEL_1, (uint32_t *)pwm_buffer, PWM_BUFFER_LEN);
}

// --- Helper function for rainbow effect ---
// Converts a hue value (0-255) to an RGB color.
static void Hue_To_RGB(uint8_t hue, uint8_t *r, uint8_t *g, uint8_t *b)
{
    if (hue < 85)
    { // Red -> Green
        *r = (85 - hue) * 3;
        *g = hue * 3;
        *b = 0;
    }
    else if (hue < 170)
    { // Green -> Blue
        hue -= 85;
        *r = 0;
        *g = (85 - hue) * 3;
        *b = hue * 3;
    }
    else
    { // Blue -> Red
        hue -= 170;
        *r = hue * 3;
        *g = 0;
        *b = (85 - hue) * 3;
    }
}

static uint16_t rainbow_hue = 0;

void RGB_RainbowCycle(void)
{
    uint8_t r, g, b;
    for (int i = 0; i < NUM_LEDS; i++)
    {
        // Calculate the hue for each LED, offset by its position to create the rainbow.
        uint8_t hue = (rainbow_hue + (i * 256 / NUM_LEDS)) & 0xFF;
        Hue_To_RGB(hue, &r, &g, &b);
        RGB_SetColor(i, r, g, b);
    }
    RGB_Update();

    // Increment the hue for the next cycle to make the rainbow move.
    rainbow_hue++;
    if (rainbow_hue >= 256)
    {
        rainbow_hue = 0;
    }
}

// 自动回调函数停止DMA
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM3)
    {
        HAL_TIM_PWM_Stop_DMA(htim, TIM_CHANNEL_1);
    }
}
