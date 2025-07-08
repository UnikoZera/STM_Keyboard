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

uint16_t filter_adc_data[4]; // 用于存储滤波后的ADC数据
uint16_t last_adc_data[4]; // 用于记录上一次的ADC数据

void ADC_Filter(void)
{
    last_adc_data[0] = filter_adc_data[0];
    last_adc_data[1] = filter_adc_data[1];
    last_adc_data[2] = filter_adc_data[2];
    last_adc_data[3] = filter_adc_data[3];

	filter_adc_data[0] = ADC_FILTER_ALPHA * buffer_adc_1[0] + (1.0f - ADC_FILTER_ALPHA) * filter_adc_data[0];
	filter_adc_data[1] = ADC_FILTER_ALPHA * buffer_adc_1[1] + (1.0f - ADC_FILTER_ALPHA) * filter_adc_data[1];
	filter_adc_data[2] = ADC_FILTER_ALPHA * buffer_adc_2[0] + (1.0f - ADC_FILTER_ALPHA) * filter_adc_data[2];
	filter_adc_data[3] = ADC_FILTER_ALPHA * buffer_adc_2[1] + (1.0f - ADC_FILTER_ALPHA) * filter_adc_data[3];
}

void ADC_Init(void) // 只需要在初始化调用这个就可以持续收集adc数据
{
    HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
    HAL_ADCEx_Calibration_Start(&hadc2, ADC_SINGLE_ENDED);
    HAL_Delay(10);
    ADC_Start_DMA();
}


void ADC_Start_DMA(void)
{
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)buffer_adc_1, sizeof(buffer_adc_1)/sizeof(uint16_t));
    HAL_ADC_Start_DMA(&hadc2, (uint32_t*)buffer_adc_2, sizeof(buffer_adc_2)/sizeof(uint16_t));
}

