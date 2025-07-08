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

void OLED_UI_Init(void);
void OLED_DisplayUI(keyboard_settings_t *settings, keyboard_state_t *state);

extern uint64_t msg_counter; // 用于动画计时


#endif /* INC_OLED_DRIVER_H_ */
