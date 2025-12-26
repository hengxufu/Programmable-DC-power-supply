#include "ds18b20.h"
#include "Delay.h"

// 引脚定义需在ds18b20.h中配置，示例：
// #define DS18B20_PORT GPIOA
// #define DS18B20_PIN  GPIO_Pin_0

// 引脚模式切换：mode=1 输入，mode=0 输出
void ds18b20_pin_in_or_out(uint8_t mode)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // 对应引脚时钟

    if(mode)
    {
        // 输入模式：浮空输入（避免内部上下拉干扰）
        GPIO_InitStruct.GPIO_Pin = DS18B20_PIN;
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(DS18B20_PORT, &GPIO_InitStruct);
    }
    else
    {
        // 输出模式：推挽输出+50MHz速度
        GPIO_InitStruct.GPIO_Pin = DS18B20_PIN;
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(DS18B20_PORT, &GPIO_InitStruct);
    }
}

// DS18B20初始化
void ds18b20_init(void)
{
    ds18b20_pin_in_or_out(0); // 初始化为输出
    GPIO_SetBits(DS18B20_PORT, DS18B20_PIN); // 引脚置高
}

// 修正版复位函数（严格遵循DS18B20时序规范）
uint8_t ds18b20_reset(void)
{
    uint8_t retry = 0;
    ds18b20_pin_in_or_out(0); // 输出模式
    GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN); // 拉低总线
    Delay_us(480); // 拉低480us（规范最小值）
    
    GPIO_SetBits(DS18B20_PORT, DS18B20_PIN); // 释放总线
    ds18b20_pin_in_or_out(1); // 立即切换输入（关键修正：原代码等100us才切）
    Delay_us(60);  // 等待传感器应答（规范60us）
    
    retry = 0;
    while(GPIO_ReadInputDataBit(DS18B20_PORT, DS18B20_PIN) && retry < 200)
    {
        retry++;
        Delay_us(1);
    }
    if(retry >= 200) return 1; // 复位失败
    Delay_us(420);        // 等待应答结束
    return 0; // 复位成功
}

// 检测DS18B20存在
uint8_t ds18b20_check(void)
{
    return ds18b20_reset(); // 复位成功返回0（存在），失败返回1（不存在）
}

// 修正版写字节函数（时序更稳定）
void ds18b20_write_byte(uint8_t dat)
{
    uint8_t i = 8;
    ds18b20_pin_in_or_out(0); // 输出模式
    while(i--)
    {
        GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN); // 拉低起始位
        Delay_us(3); // 起始位延长至3us（留余量）
        // 写数据位（0/1）
        if(dat & 0x01) GPIO_SetBits(DS18B20_PORT, DS18B20_PIN);
        else GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);
        Delay_us(60); // 数据位保持60us
        GPIO_SetBits(DS18B20_PORT, DS18B20_PIN); // 释放总线
        Delay_us(2);  // 位间隔
        dat >>= 1;    // 处理下一位
    }
}

// 修正版读字节函数（移除冗余初始化，精准采样）
uint8_t ds18b20_read_byte(void)
{
    uint8_t i = 8, dat = 0;
    while(i--)
    {
        dat >>= 1;
        ds18b20_pin_in_or_out(0); // 输出模式
        GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN); // 拉低起始位
        Delay_us(2); 
        GPIO_SetBits(DS18B20_PORT, DS18B20_PIN); // 释放总线
        Delay_us(1); // 短暂延时后切输入
        ds18b20_pin_in_or_out(1); // 输入模式
        Delay_us(10); // 采样窗口（10us，DS18B20推荐值）
        // 读取数据位
        if(GPIO_ReadInputDataBit(DS18B20_PORT, DS18B20_PIN))
        {
            dat |= 0x80;
        }
        Delay_us(50); // 剩余位时长
    }
    return dat;
}

// 启动DS18B20温度转换
void ds18b20_start(void)
{
    ds18b20_reset();
    ds18b20_write_byte(0xCC); // 跳过ROM指令
    ds18b20_write_byte(0x44); // 启动温度转换指令
}

// 修正版获取温度（核心修复：等待转换+正确解析负数）
float ds18b20_get_temp(void)
{
    uint8_t TL, TH;
    int16_t temp_raw;
    float temp_val;
    
    ds18b20_start();          // 启动温度转换
    Delay_ms(800);            // 关键：等待转换完成（≥750ms）
    ds18b20_reset();
    ds18b20_write_byte(0xCC); // 跳过ROM
    ds18b20_write_byte(0xBE); // 读取暂存器指令
    TL = ds18b20_read_byte(); // 低字节
    TH = ds18b20_read_byte(); // 高字节
    
    // 1. 正确拼接16位原始温度数据
    temp_raw = (int16_t)((TH << 8) | TL);
    // 2. 解析正负温度（DS18B20 12位补码格式）
    if((temp_raw & 0x8000) != 0) // 负数（最高位为1）
    {
        temp_raw = ~temp_raw + 1; // 补码转原码
        temp_val = -((float)temp_raw / 16.0f); // 加负号
    }
    else // 正数
    {
        temp_val = (float)temp_raw / 16.0f;
    }
    return temp_val;
}


