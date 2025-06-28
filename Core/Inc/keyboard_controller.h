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

extern USBD_HandleTypeDef hUsbDeviceFS;

void Keyboard_Init(void);
void Keyboard_Update(void);


#endif /* INC_KEYBOARD_CONTROLLER_H_ */
