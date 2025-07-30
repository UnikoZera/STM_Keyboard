/*
 * oled_driver.h
 *
 *  Created on: Jul 8, 2025
 *      Author: UnikoZera
 */

#ifndef INC_OLED_DRIVER_H_
#define INC_OLED_DRIVER_H_

#include "keyboard_controller.h"
#include "tim.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "oled.h"
#include "oled_optimize.h"
#include "oled_controller.h"

typedef enum{
    MAIN_MENU,
    RGB_MENU,
    SETTINGS_MENU,
} OLED_MenuState;

void OLED_UI_Init(void);
void OLED_DisplayUI(keyboard_settings_t *settings, keyboard_state_t *state);

void CPS_Counter(void);
void Get_OLED_Input(void);
void OLED_Drawer(void);
void OLED_UI_Loop(void);


extern uint64_t msg_counter; // 用于动画计时


#endif /* INC_OLED_DRIVER_H_ */
