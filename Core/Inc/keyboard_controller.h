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
#include "rgb_controller.h"
#include "adc_controller.h"
#include "oled.h"
#include "oled_optimize.h"
#include "usb_device.h"
#include "usbd_hid.h"
#include "keyboard_buttons.h"
#include "crc.h"

typedef struct {
    float    trigger_threshold;
    float    trigger_slope;
    uint8_t  enable_quick_trigger;
    uint8_t  rgb_style;
    uint32_t crc32;   // 用于校验数据是否有效
} keyboard_settings_t;

extern keyboard_settings_t keyboard_settings;

extern USBD_HandleTypeDef hUsbDeviceFS;

void Keyboard_Init(void);
void Keyboard_Update(void);

void Keyboard_Settings_Read(void);
void Keyboard_Settings_Save(void);


#endif /* INC_KEYBOARD_CONTROLLER_H_ */
