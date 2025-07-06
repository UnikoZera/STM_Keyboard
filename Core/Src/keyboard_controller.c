/*
 * keyboard_controller.c
 *
 *  Created on: Jun 28, 2025
 *      Author: UnikoZera
 */


#include "keyboard_controller.h"

#define KEYBOARD_TRIGGER_THRESHOLD 2000

static unsigned char keyboard_mode = 1;


// USB HID的协议是第0位为特殊按键(Left Ctrl, Left Shift, Left Alt, Left GUI, Right Ctrl, Right Shift, Right Alt, Right GUI)
// 第1位为保留位0x00
// 第2-6位为普通按键(0x00-0xFF) 这里做的是4key盘的协议
uint8_t hid_buffer[6] = {0};

void Keyboard_Init(void)
{
    ADC_Init();
    HAL_Delay(100);
    RGB_Init();
    HAL_Delay(100);
    OLED_Init();
    // USB已经在main中初始化
}

void Keyboard_Update(void)
{
    // 读取ADC数据
    ADC_Filter();

    // 读取mode按钮
    if (HAL_GPIO_ReadPin(Mode_Buttom_GPIO_Port, Mode_Buttom_Pin) == GPIO_PIN_RESET)
    {
        HAL_Delay(50); // 防抖
        if (HAL_GPIO_ReadPin(Mode_Buttom_GPIO_Port, Mode_Buttom_Pin) == GPIO_PIN_RESET)
        {
            keyboard_mode++;
            if (keyboard_mode > 2)
            {
                keyboard_mode = 1;
            }
        }
        while (HAL_GPIO_ReadPin(Mode_Buttom_GPIO_Port, Mode_Buttom_Pin) == GPIO_PIN_RESET);// 等待按键释放
    }

    if (keyboard_mode == 1)
    {
        // 读取按键状态
        hid_buffer[0] = KEYBOARD_BUTTON_NONE; // 特殊按键
        hid_buffer[1] = KEYBOARD_BUTTON_NONE; // 保留位

        if (filter_adc_1[0] > KEYBOARD_TRIGGER_THRESHOLD)
        {
            hid_buffer[2] = KEYBOARD_BUTTON_D;
        }
        else
        {
            hid_buffer[2] = KEYBOARD_BUTTON_NONE;
        }

        if (filter_adc_1[1] > KEYBOARD_TRIGGER_THRESHOLD)
        {
            hid_buffer[3] = KEYBOARD_BUTTON_F;
        }
        else
        {
            hid_buffer[3] = KEYBOARD_BUTTON_NONE;
        }
        
        if (filter_adc_2[0] > KEYBOARD_TRIGGER_THRESHOLD)
        {
            hid_buffer[4] = KEYBOARD_BUTTON_J;
        }
        else
        {
            hid_buffer[4] = KEYBOARD_BUTTON_NONE;
        }

        if (filter_adc_2[1] > KEYBOARD_TRIGGER_THRESHOLD)
        {
            hid_buffer[5] = KEYBOARD_BUTTON_K;
        }
        else
        {
            hid_buffer[5] = KEYBOARD_BUTTON_NONE;
        }

        USBD_HID_SendReport(&hUsbDeviceFS, hid_buffer, sizeof(hid_buffer));
    }
    else if (keyboard_mode == 2)
    {
        OLED_DisplayString(0, 0, "Mod Mode:");
        if (filter_adc_1[0] > KEYBOARD_TRIGGER_THRESHOLD)
        {
            // LEFT
        }

        if (filter_adc_1[1] > KEYBOARD_TRIGGER_THRESHOLD)
        {
            // UP
        }

        if (filter_adc_2[0] > KEYBOARD_TRIGGER_THRESHOLD)
        {
            // DOWN
        }

        if (filter_adc_2[1] > KEYBOARD_TRIGGER_THRESHOLD)
        {
            // RIGHT
        }
    }
}