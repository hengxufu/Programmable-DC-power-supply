#ifndef __DS18B20_H
#define __DS18B20_H

#include "stm32f10x.h"

// 引脚定义（根据你的硬件修改，示例：PA0）
#define DS18B20_PORT    GPIOB
#define DS18B20_PIN     GPIO_Pin_9

// 函数原型声明（解决223警告 + 定义接口）
void ds18b20_init(void);        // 初始化
uint8_t ds18b20_reset(void);    // 复位
void ds18b20_start(void);       // 启动温度转换
void ds18b20_write_byte(uint8_t dat); // 写一字节
uint8_t ds18b20_read_byte(void);      // 读一字节
uint8_t ds18b20_check(void);    // 检测DS18B20存在
void ds18b20_pin_in_or_out(uint8_t mode); // 引脚模式切换（1=输入，0=输出）
float ds18b20_get_temp(void);   // 获取温度值
uint32_t Get_SysTick(void);
#endif // __DS18B20_H
