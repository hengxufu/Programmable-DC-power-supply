#include "HC-06_read.h"
#include "main.h"

// 静态全局变量：仅本文件可见，避免全局暴露
static uint8_t RxData;			// 串口接收的数字（字符转数字后）
static uint8_t rx_count = 0;   // 接收计数（0-3，累计4个为一组）
static uint8_t num_buf[4] = {0};// 4位数字缓冲区
static float final_num = 0.0f;  // 最终组合的小数结果
static char result_str[16] = {0};// 串口回传用字符串

int Bluetooth_Flag=0;

/**
 * @brief  模块初始化（仅初始化串口）
 * @param  无
 * @retval 无
 */
void SerialNumParser_Init(void)
{
	Serial_Init();		// 串口初始化（需匹配硬件配置：引脚、波特率等）
}

/**
 * @brief  处理串口接收、数字校验、组合计算、串口回传
 * @param  无
 * @retval 无
 */
void SerialNumParser_Process(void)
{
	if (Serial_GetRxFlag() == 1)			// 检测到串口接收完成
	{
		if(Serial_GetRxData()==0x4A)
		{
			RxData = Serial_GetRxData();
			final_num=RxData;
			Bluetooth_Flag=1;
		}
		else
		{
		RxData = Serial_GetRxData() - 0x30;	// ASCII字符转数字（'0'-'9' → 0-9）
		Bluetooth_Flag=1;
		// 校验：仅处理0-9的有效数字，无效则重置
		 if (RxData > 9)
		{
			rx_count = 0;                  // 重置接收计数
			memset(num_buf, 0, sizeof(num_buf)); // 清空缓冲区
			Delay_ms(500);                 // 错误延时（可扩展为串口回传错误提示）
		}
		else
		{
			Serial_SendByte(RxData + 0x30); // 串口回传当前接收的数字（调试用）
			
			num_buf[rx_count] = RxData;    // 存储数字到缓冲区
			rx_count++;                    // 计数+1
			
			// 累计满4个数字，计算并输出结果
			if (rx_count >= 4)
			{
				// 组合计算：十位 + 个位 + 十分位 + 百分位（如1234 → 12.34）
				final_num = num_buf[0] * 10.0f   // 十位
						  + num_buf[1]          // 个位
						  + num_buf[2] * 0.1f   // 十分位
						  + num_buf[3] * 0.01f; // 百分位
				
				// 格式化为两位小数字符串，串口回传结果（调试用）
				snprintf(result_str, sizeof(result_str), "%.2f", final_num);
				Serial_SendString(result_str);     // 串口发送结果
				Bluetooth_Flag=1;
				// 重置，准备下一组接收
				rx_count = 0;
				memset(num_buf, 0, sizeof(num_buf));
			}
		}
	}
		Serial_ClearRxFlag(); // 清除接收标志，准备下一次接收
	}
}

/**
 * @brief  获取解析后的最终浮点数值
 * @param  无
 * @retval float 最终组合的小数结果
 */
float SerialNumParser_GetFinalNum(void)
{
	return final_num;
}
