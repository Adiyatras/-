/*
*********************************************************************************************************
*                                     MICIRUM BOARD SUPPORT PACKAGE
*
*                            (c) Copyright 2007-2008; Micrium, Inc.; Weston, FL
*
*                   All rights reserved.  Protected by international copyright laws.
*                   Knowledge of the source code may not be used to write a similar
*                   product.  This file may only be used in accordance with a license
*                   and should not be redistributed in any way.
*********************************************************************************************************
*/

#ifndef __BSP_H__
#define __BSP_H__

#include  "stm32f10x.h"
#include  "stm32f10x_conf.h" 

/* Private typedef -----------------------------------------------------------*/
// USART 
typedef enum 
{   FAILED = 0,
    PASSED = !FAILED
}   TestStatus;

/* Private define ------------------------------------------------------------*/
// BUZZER
#define       BSP_GPIOC_BUZZER          GPIO_Pin_13  // PC13(IO7) 
// MOTOR
#define		ISO_M1_L  		GPIO_ResetBits(GPIOE, GPIO_Pin_0)	// ISO_M1;
#define		ISO_M2_L 			GPIO_ResetBits(GPIOE, GPIO_Pin_1)	// ISO_M2;
#define		ISO_M3_L 			GPIO_ResetBits(GPIOE, GPIO_Pin_2)	// ISO_M3;
#define		ISO_RESET_L 	GPIO_ResetBits(GPIOE, GPIO_Pin_3)	// ISO_/RESET;
// USART 
#define BSP_IO_TXD4     GPIO_Pin_10	 // PC10(IO111)
#define BSP_IO_RXD4     GPIO_Pin_11	 // PC11(IO112)	

/* Function  -----------------------------------------------------------*/
void BSP_Configuration(void);
void BSP_GPIO_Configuration(void);
void Alarm_Ctrl(void);
void BSP_EXTI_Configuration(void);
void BSP_USART_Configuration(USART_TypeDef* USARTx);
void BSP_USART_OUT(USART_TypeDef* USARTx, uint8_t *Data,uint16_t Len);
extern void UART4_IRQHandler(void);
void BSP_RCC_Configuration(void);
void key_scan(void);
void Delay(__IO uint32_t nTime);
void TimingDelay_Decrement(void); 

#endif 
                               
