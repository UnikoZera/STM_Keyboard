/*
 * adc_controller.h
 *
 *  Created on: Jun 28, 2025
 *      Author: UnikoZera
 */

#ifndef INC_ADC_CONTROLLER_H_
#define INC_ADC_CONTROLLER_H_

#include "main.h"
#include "adc.h"
#include "dma.h"
#include <stdint.h>

#define ADC_FILTER_ALPHA 0.6f

extern uint16_t buffer_adc_1[2]; // 包含1·2
extern uint16_t buffer_adc_2[2]; // 包含3·4

extern uint16_t filter_adc_1[2]; // 包含1·2
extern uint16_t filter_adc_2[2]; // 包含3·4


void ADC_Init(void);
void ADC_Filter(void);
void ADC_Start_DMA(void); // 丢到updater中


#endif /* INC_ADC_CONTROLLER_H_ */
