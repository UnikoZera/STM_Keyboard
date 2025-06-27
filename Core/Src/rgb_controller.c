/*
 * rgb_controller.c
 *
 *  Created on: Jun 28, 2025
 *      Author: UnikoZera
 */

#include "rgb_controller.h"

#define COLOR_CALCULATION(color_value) ((color_value / RGB_MAX_BRIGHTNESS) * 200)

void RGB_Init(void)
{
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1); // Red
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2); // Green
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3); // Blue

    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1); // Red
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2); // Green
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3); // Blue

    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1); // Red
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2); // Green
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3); // Blue
}

// range: 0-255
// htim: TIM_HandleTypeDef pointer for the timer used for PWM
void RGB_SetColor(TIM_HandleTypeDef *htim, uint8_t red, uint8_t green, uint8_t blue)
{
    if (red > RGB_MAX_BRIGHTNESS) red = RGB_MAX_BRIGHTNESS;
    if (green > RGB_MAX_BRIGHTNESS) green = RGB_MAX_BRIGHTNESS;
    if (blue > RGB_MAX_BRIGHTNESS) blue = RGB_MAX_BRIGHTNESS;
    if (red < RGB_MIN_BRIGHTNESS) red = RGB_MIN_BRIGHTNESS;
    if (green < RGB_MIN_BRIGHTNESS) green = RGB_MIN_BRIGHTNESS;
    if (blue < RGB_MIN_BRIGHTNESS) blue = RGB_MIN_BRIGHTNESS;

    __HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_1, COLOR_CALCULATION(red));
    __HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_2, COLOR_CALCULATION(green));
    __HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_3, COLOR_CALCULATION(blue));
}