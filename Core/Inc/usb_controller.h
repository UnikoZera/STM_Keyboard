/*
 * usb_controller.h
 *
 *  Created on: Jun 28, 2025
 *      Author: UnikoZera
 */

#ifndef INC_USB_CONTROLLER_H_
#define INC_USB_CONTROLLER_H_

#include "main.h"
#include "gpio.h"
#include "usb_device.h"
#include "usbd_def.h"
#include <stdint.h>

void USB_Init(void);
void USB_Send_Data(uint8_t* data, uint16_t length);

#endif /* INC_USB_CONTROLLER_H_ */
