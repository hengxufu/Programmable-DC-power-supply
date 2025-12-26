#ifndef __KEY_H
#define __KEY_H	 
//	 

#include "stm32f10x.h"

//对端口进行宏定义，方面调用
#define KEY_HANG 0x000f   //0123  
#define lie1 GPIO_Pin_4   
#define lie2 GPIO_Pin_5	   
#define lie3 GPIO_Pin_6	 
#define lie4 GPIO_Pin_7   

#define lie1_Input GPIO_ReadInputDataBit(GPIOA,lie1)
#define lie2_Input GPIO_ReadInputDataBit(GPIOA,lie2)
#define lie3_Input GPIO_ReadInputDataBit(GPIOA,lie3)
#define lie4_Input GPIO_ReadInputDataBit(GPIOA,lie4)

extern u8 FLAG;   //定义一个全局便令FLAG

void KEY_4x4_Init(void);  //IO初始化
void KEY_Scan(u8 *key);   //按键扫描函数 		

#endif

