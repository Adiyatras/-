#ifndef __adc_H
#define __adc_H

#include "stm32f10x.h"

void GPIO_Configuration(void);
void ADC_Configuration(void);
void NVIC_Configuration(void);
void TIM5_Configuration(void);
extern void TIM5_IRQHandler(void);
extern void DMA1_Channel1_IRQHandler(void);

#endif

