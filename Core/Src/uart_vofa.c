/*
 * uart_vofa.c
 *
 *  Created on: Jun 7, 2025
 *      Author: UnikoZera
 */

#include "uart_vofa.h"
#include <string.h>

/**
 * @brief  发送浮点数组到VOFA+ (JustFloat协议)
 * @param  data: 浮点数数组指针
 * @param  channels: 通道数量
 */
void VOFA_SendFloat(float *data, uint8_t channels)
{
    if (data == NULL || channels == 0 || channels > VOFA_MAX_CHANNELS)
        return;

    uint8_t buffer[VOFA_MAX_CHANNELS * 4 + 4]; // 最大数据 + 帧尾
    uint8_t *ptr = buffer;

    // 将浮点数据按小端格式复制到缓冲区
    for (uint8_t i = 0; i < channels; i++)
    {
        memcpy(ptr, &data[i], sizeof(float));
        ptr += sizeof(float);
    }

    uint8_t tail[] = {VOFA_JUSTFLOAT_TAIL};
    memcpy(ptr, tail, sizeof(tail)); //直接插入就好

    HAL_UART_Transmit(&huart2, buffer, channels * sizeof(float) + sizeof(tail), 100);
}

/**
 * @brief  发送整型数组到VOFA+ (转换为浮点数发送)
 * @param  data: 整型数数组指针
 * @param  channels: 通道数量
 */
void VOFA_SendInt(int32_t *data, uint8_t channels)
{
    if (data == NULL || channels == 0 || channels > VOFA_MAX_CHANNELS)
        return;

    float float_data[VOFA_MAX_CHANNELS];

    // 将整型数据转换为浮点数(偷懒)
    for (uint8_t i = 0; i < channels; i++)
    {
        float_data[i] = (float)data[i];
    }
    VOFA_SendFloat(float_data, channels);
}

/**
 * @brief  发送单个浮点数到VOFA+
 * @param  value: 浮点数值
 */
void VOFA_SendSingleFloat(float value)
{
    VOFA_SendFloat(&value, 1);
}

/**
 * @brief  发送单个整型数到VOFA+
 * @param  value: 整型数值
 */
void VOFA_SendSingleInt(int32_t value)
{
    VOFA_SendInt(&value, 1);
}

/**
 * @brief  发送混合数据到VOFA+ (浮点数+整型数)
 * @param  float_data: 浮点数数组指针
 * @param  float_channels: 浮点数通道数量
 * @param  int_data: 整型数数组指针
 * @param  int_channels: 整型数通道数量
 * 先浮点，后整型
 */
void VOFA_SendMixedData(float *float_data, uint8_t float_channels, int32_t *int_data, uint8_t int_channels)
{
    uint8_t total_channels = float_channels + int_channels;

    if (total_channels == 0 || total_channels > VOFA_MAX_CHANNELS)
        return;

    float mixed_data[VOFA_MAX_CHANNELS];
    uint8_t index = 0;

    if (float_data != NULL && float_channels > 0)
    {
        for (uint8_t i = 0; i < float_channels; i++)
        {
            mixed_data[index++] = float_data[i];
        }
    }

    if (int_data != NULL && int_channels > 0)
    {
        for (uint8_t i = 0; i < int_channels; i++)
        {
            mixed_data[index++] = (float)int_data[i];
        }
    }

    // 发送混合数据
    VOFA_SendFloat(mixed_data, total_channels);
}
