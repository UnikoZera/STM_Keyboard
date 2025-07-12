/*
 * oled_driver.c
 *
 *  Created on: Jul 8, 2025
 *      Author: UnikoZera
 */

#include "oled_driver.h"

uint16_t cps = 0;
uint64_t msg_counter = 0; // 用于动画计时

void OLED_UI_Init(void)
{
    OLED_InitAnimationManager(&Menu_AnimationManager);
    OLED_InitAnimationManager(&g_Title_AnimationManager);
    OLED_InitAnimationManager(&g_AnimationManager);
}

void OLED_DisplayUI(keyboard_settings_t *settings, keyboard_state_t *state)
{
    OLED_ClearBuffer(); // 清除OLED缓冲区

    if (settings->keyboard_mode == 1)
    {
        // 画个好看的UI和ICON就可
    }
    else if (settings->keyboard_mode == 2)
    {
        // 画个RGB灯光效果
    }
    else if (settings->keyboard_mode == 3)
    {
        // 画个调整阈值的UI
    }
    

    OLED_UpdateAnimationManager(&Menu_AnimationManager); // 更新动画管理器
    OLED_UpdateAnimationManager(&g_Title_AnimationManager); // 更新标题动画管理器
    OLED_UpdateAnimationManager(&g_AnimationManager); // 更新全局动画管理器
    OLED_SmartUpdate(); // 使用智能更新模式
}

void CPS_Counter(void)
{
    static uint64_t last_counter = 0;

    if (msg_counter - last_counter >= 100) // 每100ms计算一次CPS
    {
        cps = (uint16_t) ((msg_counter - last_counter) * 10);
        last_counter = msg_counter;
    }
}