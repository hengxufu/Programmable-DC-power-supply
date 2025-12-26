#include "stm32f10x.h"                  // Device header
#include "Delay.h"

/**
  * 函    数：标识初始化
  * 参    数：无
  * 返 回 值：无
  */
void Key_Init(void)
{
	/*开启时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);		//开启GPIOB的时钟
	
	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);						//将PB1引脚初始化为上拉输入
}

/**
* 函    数：过载标识或缺
  * 参    数：无
  * 返 回 值：范围：0~1，返回0代表没有过载
  */

uint8_t Key_GetNum(void)
{
	uint8_t KeyNum = 0;		//定义变量，默认键码值为0
	
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 1)			//读PB1输入寄存器的状态，如果为0，则代表按键1按下
	{
						KeyNum = 1;												//置键码为1
	}
	return KeyNum;			//返回键码值，如果没有按键按下，所有if都不成立，则键码为默认值0
}
