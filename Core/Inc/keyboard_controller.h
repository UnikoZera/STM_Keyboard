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

#define DEFAULT_TRIGGER_POSITION_THRESHOLD 1000
#define DEFAULT_TRIGGER_SPEED_THRESHOLD 100
#define DEFAULT_RELEASE_SPEED_THRESHOLD 50

typedef struct {
    uint8_t  keyboard_mode; // 1是键盘输出模式、2是RGB灯光模式、3是调整键盘阈值大小和开启快速触发模式
    uint8_t  rgb_style;
    uint8_t  enable_quick_trigger;

    // keyboard trigger settings
    uint16_t _1trigger_position_threshold;
    uint16_t _1trigger_speed_threshold;
    uint16_t _1release_speed_threshold;

    uint16_t _2trigger_position_threshold;
    uint16_t _2trigger_speed_threshold;
    uint16_t _2release_speed_threshold;

    uint16_t _3trigger_position_threshold;
    uint16_t _3trigger_speed_threshold;
    uint16_t _3release_speed_threshold;

    uint16_t _4trigger_position_threshold;
    uint16_t _4trigger_speed_threshold;
    uint16_t _4release_speed_threshold;

    uint8_t  padding[5];
    // 注意: 这里的padding是为了确保结构体大小为8的倍数
    uint32_t crc32;   // 用于校验数据是否有效
} keyboard_settings_t;

typedef enum {
    KEY_PRESSING,   // 按下过程中
    KEY_PRESSED,    // 已按下（稳定触发）
    KEY_RELEASING,  // 释放过程中
    KEY_RELEASED    // 已释放
} magnetic_key_state_t;

typedef struct{
    magnetic_key_state_t key_state;         // 当前状态
    uint16_t trigger_position_threshold;    // 触发位置阈值
    uint16_t trigger_speed_threshold;       // 触发速度阈值
    uint16_t release_speed_threshold;       // 释放速度阈值
    uint32_t press_time;                    // 按下时间戳
    int16_t press_speed;                    // 按下速度
} magnetic_key_info_t;

typedef struct {
    magnetic_key_info_t KEY_1_info;
    magnetic_key_info_t KEY_2_info;
    magnetic_key_info_t KEY_3_info;
    magnetic_key_info_t KEY_4_info;

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
