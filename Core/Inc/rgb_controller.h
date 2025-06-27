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

#define RGB_MAX_BRIGHTNESS 255
#define RGB_MIN_BRIGHTNESS 0

// 5khz PWM frequency
void RGB_Init(void);
void RGB_SetColor(TIM_HandleTypeDef *htim, uint8_t red, uint8_t green, uint8_t blue);

#endif /* INC_RGB_CONTROLLER_H_ */
