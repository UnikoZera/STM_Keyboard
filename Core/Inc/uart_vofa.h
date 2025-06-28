/*
 * uart_vofa.h
 *
 *  Created on: Jun 7, 2025
 *      Author: UnikoZera
 */

#ifndef INC_UART_VOFA_H_
#define INC_UART_VOFA_H_

#include "main.h"
#include "usart.h"
#include <stdint.h>

/* VOFA+ 协议定义 */
#define VOFA_JUSTFLOAT_TAIL 0x00, 0x00, 0x80, 0x7F // JustFloat协议帧尾
#define VOFA_MAX_CHANNELS 16                       // 最大通道数

/* 函数声明 */
void VOFA_SendFloat(float *data, uint8_t channels);
void VOFA_SendInt(int32_t *data, uint8_t channels);
void VOFA_SendSingleFloat(float value);
void VOFA_SendSingleInt(int32_t value);
void VOFA_SendMixedData(float *float_data, uint8_t float_channels, int32_t *int_data, uint8_t int_channels);

/* 使用示例:
 *
 * 1. 发送单个浮点数:
 *    VOFA_SendSingleFloat(3.14f);
 *
 * 2. 发送单个整数:
 *    VOFA_SendSingleInt(1234);
 *
 * 3. 发送多个浮点数:
 *    float data[] = {1.1f, 2.2f, 3.3f};
 *    VOFA_SendFloat(data, 3);
 *
 * 4. 发送多个整数:
 *    int32_t data[] = {100, 200, 300};
 *    VOFA_SendInt(data, 3);
 *
 * 5. 发送混合数据:
 *    float f_data[] = {1.1f, 2.2f};
 *    int32_t i_data[] = {100, 200};
 *    VOFA_SendMixedData(f_data, 2, i_data, 2);
 */

#endif /* INC_UART_VOFA_H_ */
