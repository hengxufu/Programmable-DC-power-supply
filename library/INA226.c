#include "ina226.h"

/* ************************** 延时函数 ************************** */
/**
 * @brief  纳秒级延时函数（适配IIC高速模式时序需求）
 * @param  D: 延时循环计数（需根据实际主频校准）
 * @retval 无
 */
static void delay_nns(uint16_t D)
{
    while (--D);
}

/**
 * @brief  毫秒级延时函数
 * @param  ms: 延时毫秒数
 * @retval 无
 */
void delay_nms(uint16_t ms)
{
    uint16_t i;
    for (i = 0; i < ms; i++)
    {
        for (uint32_t M = 12000; M > 0; M--);
    }
}

/**
 * @brief  微秒级延时函数
 * @param  us: 延时微秒数
 * @retval 无
 */
void delay_nus(uint16_t us)
{
    uint16_t i;
    for (i = 0; i < us; i++)
    {
        for (uint16_t M = 72; M > 0; M--);
    }
}

/* ************************** IIC底层驱动 ************************** */
/**
 * @brief  INA226 对应的IIC总线初始化
 * @param  无
 * @retval 无
 * @note   采用软件IIC，GPIO配置为开漏输出，需外部上拉电阻
 */
void INA226_IIC_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // 使能GPIO时钟
    RCC_APB2PeriphClockCmd(INA_SCL_RCC | INA_SDA_RCC, ENABLE);

    // 配置SCL引脚
    GPIO_InitStructure.GPIO_Pin = INA_SCL_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  // 开漏输出，兼容IIC总线特性
    GPIO_Init(INA_SCL_GPIO_PORT, &GPIO_InitStructure);

    // 配置SDA引脚
    GPIO_InitStructure.GPIO_Pin = INA_SDA_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  // 开漏输出，需要外部上拉电阻
    GPIO_Init(INA_SDA_GPIO_PORT, &GPIO_InitStructure);

    // 初始化为高电平（IIC总线空闲状态）
    INA_SCL = 1;
    INA_SDA = 1;
    delay_nms(5);  // 稳定延时
}

/**
 * @brief  IIC起始信号
 * @param  无
 * @retval 无
 * @note   时序：时钟高电平时，数据从高到低跳变
 */
void INA226_IIC_Start(void)
{
    INA_SCL = 1;
    INA_SDA = 1;
    delay_nns(5);
    INA_SDA = 0;  // START: 时钟高电平时数据从高到低
    delay_nns(5);
    INA_SCL = 0;  // 钳住总线准备传输数据
    delay_nns(5);
}

/**
 * @brief  IIC停止信号
 * @param  无
 * @retval 无
 * @note   时序：时钟高电平时，数据从低到高跳变
 */
void INA226_IIC_Stop(void)
{
    INA_SDA = 0;  // STOP: 时钟高电平时数据从低到高
    delay_nns(5);
    INA_SCL = 1;
    delay_nns(5);
    INA_SDA = 1;  // 发送总线结束信号
    delay_nns(5);
}

/**
 * @brief  IIC发送应答信号
 * @param  无
 * @retval 无
 */
void INA226_IIC_Ack(void)
{
    INA_SDA = 0;
    delay_nns(5);
    INA_SCL = 1;
    delay_nns(5);
    INA_SCL = 0;
    delay_nns(5);
    INA_SDA = 1;
}

/**
 * @brief  IIC发送非应答信号
 * @param  无
 * @retval 无
 */
void INA226_IIC_NAck(void)
{
    INA_SDA = 1;
    delay_nns(5);
    INA_SCL = 1;
    delay_nns(5);
    INA_SCL = 0;
    delay_nns(5);
    INA_SDA = 0;
}

/**
 * @brief  等待IIC应答信号
 * @param  无
 * @retval 0: 收到应答  1: 超时无应答
 */
uint8_t INA226_IIC_Wait_Ack(void)
{
    uint8_t ucErrTime = 0;

    INA_SDA = 1;
    delay_nns(5);
    INA_SCL = 1;
    delay_nns(5);

    while (READ_SDA)
    {
        ucErrTime++;
        if (ucErrTime > 250)
        {
            INA226_IIC_Stop();
            return 1;  // 超时无应答，停止总线
        }
    }

    INA_SCL = 0;  // 时钟拉低，准备后续传输
    return 0;      // 收到应答
}

/**
 * @brief  IIC发送一个字节
 * @param  txd: 待发送的字节
 * @retval 无
 */
void INA226_IIC_Send_Byte(uint8_t txd)
{
    int i;

    INA_SCL = 0;  // 拉低时钟开始传输
    for (i = 0; i < 8; i++)
    {
        INA_SDA = (txd & 0x80) ? 1 : 0;  // 高位先行
        txd <<= 1;
        INA_SCL = 1;                     // 时钟上升沿锁存数据
        delay_nns(5);
        INA_SCL = 0;                     // 拉低时钟准备下一位
        delay_nns(5);
    }
}

/**
 * @brief  IIC读取一个字节
 * @param  ack: 应答标志（1:发送ACK  0:发送NACK）
 * @retval 读取到的字节
 */
uint8_t INA226_IIC_Read_Byte(unsigned char ack)
{
    uint8_t TData = 0, i;

    for (i = 0; i < 8; i++)
    {
        INA_SCL = 1;
        delay_nns(5);
        TData = TData << 1;
        if (READ_SDA)
        {
            TData |= 0x01;  // 读取当前SDA电平
        }
        INA_SCL = 0;
        delay_nns(5);
    }

    if (!ack)
    {
        INA226_IIC_NAck();  // 发送非应答
    }
    else
    {
        INA226_IIC_Ack();   // 发送应答
    }

    return TData;
}

/* ************************** INA226核心驱动 ************************** */
/**
 * @brief  初始化第一个INA226（地址WRITE_ADDR）
 * @param  无
 * @retval 无
 */
void INA226_Init(void)
{
    uint16_t Cal;

    INA226_IIC_Init();
    delay_nms(10);  // 芯片上电稳定延时

    // 配置寄存器：连续测量模式，转换时间0.14ms，平均值128
    INA226_SendData(WRITE_ADDR, 0x00, 0x4807);

    // 计算电流分辨率（基于配置的最大电流动态计算）
    Current_LSB = (float)MAX_CURRENT / 32768;
    // 计算校准值（0.00512是INA226手册规定的校准系数）
    Cal = (uint16_t)(0.00512f / (Current_LSB * SHUNT_RESISTOR));
    // 写入校准值到校准寄存器
    INA226_SendData(WRITE_ADDR, 0x05, Cal);

    // 计算功率分辨率（功率LSB = 电流LSB * 25）
    Power_LSB = Current_LSB * 25.0f;
}

/**
 * @brief  初始化第二个INA226（地址WRITE_ADDR1）
 * @param  无
 * @retval 无
 */
void INA226_Init1(void)
{
    uint16_t Cal;

    INA226_IIC_Init();
    delay_nms(10);  // 芯片上电稳定延时

    // 配置寄存器：连续测量模式，转换时间0.14ms，平均值128
    INA226_SendData(WRITE_ADDR1, 0x00, 0x4807);

    // 计算电流分辨率
    Current_LSB = (float)MAX_CURRENT / 32768;
    // 计算校准值
    Cal = (uint16_t)(0.00512f / (Current_LSB * SHUNT_RESISTOR));
    // 写入校准值
    INA226_SendData(WRITE_ADDR1, 0x05, Cal);

    // 计算功率分辨率
    Power_LSB = Current_LSB * 25.0f;
}

/**
 * @brief  向INA226指定寄存器写入16位数据
 * @param  addr: 设备写地址
 * @param  reg: 寄存器地址
 * @param  data: 待写入的16位数据
 * @retval 无
 */
void INA226_SendData(uint8_t addr, uint8_t reg, uint16_t data)
{
    uint8_t temp = 0;

    INA226_IIC_Start();
    INA226_IIC_Send_Byte(addr);
    if (INA226_IIC_Wait_Ack()) return;  // 等待应答，失败则退出

    INA226_IIC_Send_Byte(reg);
    if (INA226_IIC_Wait_Ack()) return;

    temp = (uint8_t)(data >> 8);  // 拆分高8位
    INA226_IIC_Send_Byte(temp);
    if (INA226_IIC_Wait_Ack()) return;

    temp = (uint8_t)(data & 0x00FF);  // 拆分低8位
    INA226_IIC_Send_Byte(temp);
    if (INA226_IIC_Wait_Ack()) return;

    INA226_IIC_Stop();
}

/**
 * @brief  设置INA226寄存器指针（指定下次读取的寄存器）
 * @param  addr: 设备写地址
 * @param  reg: 寄存器地址
 * @retval 无
 */
void INA226_SetRegPointer(uint8_t addr, uint8_t reg)
{
    INA226_IIC_Start();
    INA226_IIC_Send_Byte(addr);
    if (INA226_IIC_Wait_Ack()) return;

    INA226_IIC_Send_Byte(reg);
    if (INA226_IIC_Wait_Ack()) return;

    INA226_IIC_Stop();
}

/**
 * @brief  从INA226读取16位数据
 * @param  addr: 设备写地址
 * @retval 读取到的16位数据
 */
uint16_t INA226_ReadData(uint8_t addr)
{
    uint16_t temp = 0;

    INA226_IIC_Start();
    INA226_IIC_Send_Byte(addr + 1);  // 读地址 = 写地址 + 1
    if (INA226_IIC_Wait_Ack()) return 0;

    temp = INA226_IIC_Read_Byte(1);  // 读高8位，发送应答
    temp <<= 8;
    temp |= INA226_IIC_Read_Byte(0); // 读低8位，发送非应答

    INA226_IIC_Stop();
    return temp;
}

/**
 * @brief  获取分流电流原始值（处理符号位）
 * @param  addr: 设备写地址
 * @retval 电流原始值（有符号处理）
 */
uint16_t INA226_GetShuntCurrent(uint8_t addr)
{
    uint16_t temp = 0;

    INA226_SetRegPointer(addr, Current_Reg);
    temp = INA226_ReadData(addr);

    // 处理补码（负数转换）
    if (temp & 0x8000)
    {
        temp = (uint16_t)(-(int16_t)(~temp + 1));
    }

    return temp;
}

/**
 * @brief  获取分流电压原始值（2.5uV/bit）
 * @param  addr: 设备写地址
 * @retval 分流电压原始值
 */
uint16_t INA226_GetShuntVoltage(uint8_t addr)
{
    uint16_t temp = 0;

    INA226_SetRegPointer(addr, Shunt_V_Reg);
    temp = INA226_ReadData(addr);

    // 处理负数补码
    if (temp & 0x8000)
    {
        temp = (uint16_t)(-(int16_t)(~temp + 1));
    }

    return temp;
}

/**
 * @brief  获取总线电压原始值（1.25mV/bit）
 * @param  addr: 设备写地址
 * @retval 总线电压原始值
 */
uint16_t INA226_GetVoltage(uint8_t addr)
{
    uint16_t temp = 0;

    INA226_SetRegPointer(addr, Bus_V_Reg);
    temp = INA226_ReadData(addr);

    return temp;
}

/**
 * @brief  获取功率原始值
 * @param  addr: 设备写地址
 * @retval 功率原始值
 */
uint16_t INA226_Get_Power(uint8_t addr)
{
    uint16_t temp = 0;

    INA226_SetRegPointer(addr, Power_Reg);
    temp = INA226_ReadData(addr);

    return temp;
}

/**
 * @brief  获取芯片ID
 * @param  addr: 设备写地址
 * @retval 芯片ID值
 */
uint16_t INA226_Get_ID(uint8_t addr)
{
    uint16_t temp = 0;

    INA226_SetRegPointer(addr, ID_Reg);
    temp = INA226_ReadData(addr);

    return temp;
}

/**
 * @brief  获取校准寄存器值（用于调试）
 * @param  addr: 设备写地址
 * @retval 校准寄存器值
 */
uint16_t INA226_GET_CAL_REG(uint8_t addr)
{
    uint16_t temp = 0;

    INA226_SetRegPointer(addr, Calib_Reg);
    temp = INA226_ReadData(addr);

    return temp;
}

/**
 * @brief  读取电流值（单位：A）
 * @param  addr: 设备写地址
 * @retval 电流值（A）
 */
float INA226_ReadCurrent_A(uint8_t addr)
{
    int16_t raw = (int16_t)INA226_GetShuntCurrent(addr);  // 有符号类型处理负数电流
    return raw * Current_LSB;
}

/**
 * @brief  读取电流值（单位：mA）
 * @param  addr: 设备写地址
 * @retval 电流值（mA）
 */
float INA226_ReadCurrent_mA(uint8_t addr)
{
    return INA226_ReadCurrent_A(addr) * 1000.0f;
}

/**
 * @brief  读取功率值（单位：W）
 * @param  addr: 设备写地址
 * @retval 功率值（W）
 */
float INA226_ReadPower(uint8_t addr)
{
    uint16_t raw = INA226_Get_Power(addr);
    return raw * Power_LSB;
}