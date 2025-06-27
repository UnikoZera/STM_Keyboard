/*
 * keyboard_controller.c
 *
 *  Created on: Jun 28, 2025
 *      Author: UnikoZera
 */


#include "keyboard_controller.h"

void Keyboard_Init(void)
{
    ADC_Init();
    HAL_Delay(100);
    RGB_Init();
    HAL_Delay(100);
    USB_Init();
    HAL_Delay(100);
    OLED_Init();
}

void Keyboard_Update(void)
{
    
}