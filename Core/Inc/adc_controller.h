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

extern uint16_t buffer_adc_1[2]; // 包含1·2
extern uint16_t buffer_adc_2[2]; // 包含3·4

extern uint16_t filter_adc_data[4];
extern uint16_t last_adc_data[4]; // 用于快速触发模式的上次ADC数据


void ADC_Init(void);
void ADC_Filter(void);
void ADC_Start_DMA(void);


#endif /* INC_ADC_CONTROLLER_H_ */
