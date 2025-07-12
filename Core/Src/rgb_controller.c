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

static led_fade_t led_fade_states[NUM_LEDS];
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

void RGB_InitFade(uint8_t led_index)
{
    if (led_index < NUM_LEDS)
    {
        if (led_fade_states[led_index].is_first_trigger == 1)
        {
            led_fade_states[led_index].is_first_trigger = 0;
            led_fade_states[led_index].current_r = rand() % 256;
            led_fade_states[led_index].current_g = rand() % 256;
            led_fade_states[led_index].current_b = rand() % 256;
        }

        led_fade_states[led_index].target_r = 0;
        led_fade_states[led_index].target_g = 0;
        led_fade_states[led_index].target_b = 0;
        led_fade_states[led_index].is_fading = 0;
        led_fade_states[led_index].fade_step = 5;
    }
}

// 触发LED渐暗效果
void RGB_TriggerFade(uint8_t led_index, uint8_t target_r, uint8_t target_g, uint8_t target_b)
{
    if (led_index < NUM_LEDS)
    {
        // 设置目标颜色
        led_fade_states[led_index].target_r = target_r;
        led_fade_states[led_index].target_g = target_g;
        led_fade_states[led_index].target_b = target_b;

        // 开始渐暗
        led_fade_states[led_index].is_fading = 1;
    }
}

// 处理渐暗效果
void RGB_ProcessFade(void)
{    
    for (int i = 0; i < NUM_LEDS; i++)
    {
        if (led_fade_states[i].is_fading)
        {
            // 渐暗处理
            if (led_fade_states[i].current_r + led_fade_states[i].target_r > led_fade_states[i].fade_step)
                led_fade_states[i].current_r -= led_fade_states[i].fade_step;
            else
                led_fade_states[i].current_r = 0;

            if (led_fade_states[i].current_g + led_fade_states[i].target_g > led_fade_states[i].fade_step)
                led_fade_states[i].current_g -= led_fade_states[i].fade_step;
            else
                led_fade_states[i].current_g = 0;

            if (led_fade_states[i].current_b + led_fade_states[i].target_b > led_fade_states[i].fade_step)
                led_fade_states[i].current_b -= led_fade_states[i].fade_step;
            else
                led_fade_states[i].current_b = 0;
            
            // 设置LED颜色
            RGB_SetColor(i, led_fade_states[i].current_r, 
                           led_fade_states[i].current_g, 
                           led_fade_states[i].current_b);
            
            // 检查是否渐暗完成
            if (led_fade_states[i].current_r == led_fade_states[i].target_r && 
                led_fade_states[i].current_g == led_fade_states[i].target_g && 
                led_fade_states[i].current_b == led_fade_states[i].target_b)
            {
                led_fade_states[i].is_fading = 0;
                led_fade_states[i].is_first_trigger = 1;
            }
        }
    }
}


void RGB_Loop(void)
{
    if (keyboard_settings.rgb_style == 1)
    {
        RGB_RainbowCycle();
        HAL_Delay(100); // 控制速度
    }
    else if (keyboard_settings.rgb_style == 2)
    {
        if (keyboard_state.KEY_1_info.key_state == KEY_PRESSED)
        {
            RGB_InitFade(0); // 触发LED 1渐暗
        }
        else if (keyboard_state.KEY_1_info.key_state == KEY_RELEASED)
        {
            led_fade_states[0].is_first_trigger = 1;
            RGB_TriggerFade(0, 0, 0, 0);
        }

        if (keyboard_state.KEY_2_info.key_state == KEY_PRESSED)
        {
            RGB_InitFade(1); // 触发LED 2渐暗
        }
        else if (keyboard_state.KEY_2_info.key_state == KEY_RELEASED)
        {
            led_fade_states[1].is_first_trigger = 1;
            RGB_TriggerFade(1, 0, 0, 0);
        }

        if (keyboard_state.KEY_3_info.key_state == KEY_PRESSED)
        {
            RGB_InitFade(2); // 触发LED 3渐暗
        }
        else if (keyboard_state.KEY_3_info.key_state == KEY_RELEASED)
        {
            led_fade_states[2].is_first_trigger = 1;
            RGB_TriggerFade(2, 0, 0, 0);
        }

        if (keyboard_state.KEY_4_info.key_state == KEY_PRESSED)
        {
            RGB_InitFade(3); // 触发LED 4渐暗
        }
        else if (keyboard_state.KEY_4_info.key_state == KEY_RELEASED)
        {
            led_fade_states[3].is_first_trigger = 1;
            RGB_TriggerFade(3, 0, 0, 0);
        }

        RGB_ProcessFade(); // 处理渐暗效果
        RGB_Update(); // 更新LED颜色
        HAL_Delay(50); // 控制渐暗速度
    }
    else if (keyboard_settings.rgb_style == 0)
    {
        RGB_Clear(); // 清除颜色
        RGB_Update(); // 更新LED颜色
        HAL_Delay(100); // 控制速度
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