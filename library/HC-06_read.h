#ifndef SERIAL_NUM_PARSER_H
#define SERIAL_NUM_PARSER_H

#include "stm32f10x.h"

extern int Bluetooth_Flag;

/**
 * @brief  串口数字解析模块初始化（仅初始化串口）
 * @param  无
 * @retval 无
 */
void SerialNumParser_Init(void);

/**
 * @brief  串口数字解析核心处理函数（主循环中调用）
 * @param  无
 * @retval 无
 */
void SerialNumParser_Process(void);

/**
 * @brief  获取解析后的最终浮点数值
 * @param  无
 * @retval float 解析完成的小数结果（如接收1234则返回12.34）
 */
float SerialNumParser_GetFinalNum(void);

#endif // SERIAL_NUM_PARSER_H
