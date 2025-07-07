/*
 * rgb_controller.h
 *
 *  Created on: Jun 28, 2025
 *      Author: UnikoZera
 */

#ifndef INC_RGB_CONTROLLER_H_
#define INC_RGB_CONTROLLER_H_

#include "main.h"
#include "tim.h"
#include "gpio.h"
#include <stdint.h>

// --- Constants ---
#define NUM_LEDS 4

// --- Function Prototypes ---
void RGB_Init(void);
void RGB_SetColor(uint8_t led_index, uint8_t red, uint8_t green, uint8_t blue);
void RGB_Clear(void);
void RGB_Update(void);
void RGB_RainbowCycle(void);

#endif /* INC_RGB_CONTROLLER_H_ */
