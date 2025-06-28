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

#define NUM_LEDS 4
#define BITS_PER_LED 24
#define WS2812_BUFFER_SIZE (NUM_LEDS * BITS_PER_LED)

#define RGB_MAX_BRIGHTNESS 255
#define RGB_MIN_BRIGHTNESS 0

void RGB_Init(void);
void RGB_SetColor(uint8_t led_index, uint8_t red, uint8_t green, uint8_t blue);
void RGB_Update(void);

#endif /* INC_RGB_CONTROLLER_H_ */
