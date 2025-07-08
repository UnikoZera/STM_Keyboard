/*
 * keyboard_controller.h
 *
 *  Created on: Jun 28, 2025
 *      Author: UnikoZera
 */

#ifndef INC_KEYBOARD_CONTROLLER_H_
#define INC_KEYBOARD_CONTROLLER_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "main.h"
#include "oled.h"
#include "oled_optimize.h"
#include "oled_controller.h"
#include "rgb_controller.h"
#include "adc_controller.h"
#include "usb_device.h"
#include "usbd_hid.h"
#include "keyboard_buttons.h"
#include "crc.h"

typedef struct {
    float    trigger_threshold;
    float    trigger_slope;
    uint8_t  enable_quick_trigger;
    uint8_t  rgb_style;
    uint8_t  keyboard_mode; // 1是键盘输出模式、2是RGB灯光模式、3是调整键盘阈值大小和开启快速触发模式
    uint8_t  padding[5];
    // 注意: 这里的padding是为了确保结构体大小为8的倍数
    uint32_t crc32;   // 用于校验数据是否有效
} keyboard_settings_t;

typedef struct {
    bool KEY_1;
    bool KEY_2;
    bool KEY_3;
    bool KEY_4;

    bool TouchButton_1;
    bool TouchButton_2;
} keyboard_state_t;


extern USBD_HandleTypeDef hUsbDeviceFS;
extern keyboard_settings_t keyboard_settings;
extern keyboard_state_t keyboard_state;

void Keyboard_Init(void);
void Handle_Mode_Switch(keyboard_settings_t *settings, keyboard_state_t *state);
void Keyboard_Read_Input(keyboard_settings_t *settings, keyboard_state_t *state);
void Keyboard_Updater(keyboard_settings_t *settings, keyboard_state_t *state);

void Keyboard_Settings_Read(void);
void Keyboard_Settings_Save(void);


#endif /* INC_KEYBOARD_CONTROLLER_H_ */
