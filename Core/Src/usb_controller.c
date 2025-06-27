/*
 * usb_controller.c
 *
 *  Created on: Jun 28, 2025
 *      Author: UnikoZera
 */


#include "usb_controller.h"

void USB_Init(void)
{
    // MX_USB_DEVICE_Init(); // 已经在main中初始化
    HAL_Delay(100);

}

void USB_Send_Data(uint8_t* data, uint16_t length)
{

}