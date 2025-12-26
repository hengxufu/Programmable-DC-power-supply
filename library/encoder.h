#ifndef ENCODER_H
#define ENCODER_H

#include "stm32f10x.h"

// ????????(GPIOA)
#define Encoder_GPIO_CLK        RCC_APB2Periph_GPIOA
#define Encoder_GPIO_PORT       GPIOA
#define Encoder_GPIO_PORT_Source GPIO_PortSourceGPIOA
#define Encoder_GPIO_PIN_CLK    GPIO_Pin_0  // CLK ? PA0
#define Encoder_GPIO_PIN_DT     GPIO_Pin_1  // DT ? PA1
#define Encoder_GPIO_PIN_SW     GPIO_Pin_2  // SW ? PA2

// ????
void Encoder_Init(void);
int16_t Encoder_Get_Count(void);
extern int Encoder_Flag;
// ????????(???????)
void EXTI0_IRQHandler(void);
void EXTI1_IRQHandler(void);

#endif /* ENCODER_H */
