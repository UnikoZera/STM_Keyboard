/*
 * adc_controller.c
 *
 *  Created on: Jun 28, 2025
 *      Author: UnikoZera
 */

#include "adc_controller.h"


#define ADC_FILTER_ALPHA 0.6f

uint16_t buffer_adc_1[2];
uint16_t buffer_adc_2[2];

uint16_t filter_adc_1[2];
uint16_t filter_adc_2[2];

void ADC_Filter(void)
{
	filter_adc_1[0] = ADC_FILTER_ALPHA * buffer_adc_1[0] + (1.0f - ADC_FILTER_ALPHA) * filter_adc_1[0];
	filter_adc_1[1] = ADC_FILTER_ALPHA * buffer_adc_1[1] + (1.0f - ADC_FILTER_ALPHA) * filter_adc_1[1];

	filter_adc_2[0] = ADC_FILTER_ALPHA * buffer_adc_2[0] + (1.0f - ADC_FILTER_ALPHA) * filter_adc_2[0];
	filter_adc_2[1] = ADC_FILTER_ALPHA * buffer_adc_2[1] + (1.0f - ADC_FILTER_ALPHA) * filter_adc_2[1];
}

void ADC_Init(void)
{
    HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
    HAL_ADCEx_Calibration_Start(&hadc2, ADC_SINGLE_ENDED);
}


void ADC_Start_DMA(void)
{
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)buffer_adc_1, sizeof(buffer_adc_1)/sizeof(uint16_t));
    HAL_ADC_Start_DMA(&hadc2, (uint32_t*)buffer_adc_2, sizeof(buffer_adc_2)/sizeof(uint16_t));
}

