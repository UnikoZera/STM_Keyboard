/*
 * rgb_controller.h
 *
 *  Created on: Jun 28, 2025
 *      Author: UnikoZera
 */

#ifndef INC_RGB_CONTROLLER_H_
#define INC_RGB_CONTROLLER_H_

#include "main.h"
#include "tim.h"
#include "gpio.h"
#include "keyboard_controller.h"
#include <math.h>
#include <stdint.h>

// --- Constants ---
#define NUM_LEDS 4

typedef struct {
    uint8_t target_r, target_g, target_b;  // 目标颜色
    uint8_t current_r, current_g, current_b;  // 当前颜色
    uint8_t fade_step;  // 渐暗步长
    uint8_t is_fading;  // 是否正在渐暗
    uint8_t is_first_trigger;
} led_fade_t;

// --- Function Prototypes ---
void RGB_Init(void);
void RGB_SetColor(uint8_t led_index, uint8_t red, uint8_t green, uint8_t blue);
void RGB_Clear(void);
void RGB_Update(void);
void RGB_RainbowCycle(void);

void RGB_TriggerFade(uint8_t led_index, uint8_t target_r, uint8_t target_g, uint8_t target_b);
void RGB_InitFade(uint8_t led_index); // 初始化渐暗状态

void RGB_ProcessFade(void); // 处理渐暗效果

void RGB_Loop(void); // RGB灯光循环

#endif /* INC_RGB_CONTROLLER_H_ */
