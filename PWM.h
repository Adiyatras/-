#ifndef __PWM_INIT_H__
#define __PWM_INIT__

#define uint8 unsigned char
#define uint16 unsigned int
#define uint32 unsigned long
#define u8 unsigned char
#define u16 unsigned int
#define u32 unsigned long
	

#define 	PWMA_DIS	TIM_CCxNCmd(TIM1, TIM_Channel_3, TIM_CCxN_Disable)// 禁止使能TIMx在CCR3 PE_12 T1_CH3N  PWMA ;
#define 	PWMB_DIS	TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Disable) 	// 禁止使能TIMx在CCR1 PE_9  T1_CH1   PWMB ;
#define 	PWMC_DIS	TIM_CCxNCmd(TIM1, TIM_Channel_1, TIM_CCxN_Disable)// 禁止使能TIMx在CCR1 PE_8  T1_CH1N  PWMC ;
#define 	PWMD_DIS	TIM_CCxCmd(TIM1, TIM_Channel_2, TIM_CCx_Disable)  // 禁止使能TIMx在CCR2 PE_11 T1_CH2   PWMD ;

#define 	PWMA_EN   TIM_CCxNCmd(TIM1, TIM_Channel_3, TIM_CCxN_Enable)	// 使能TIMx在CCR3 PE_12 T1_CH3N  PWMA ;
#define 	PWMB_EN   TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Enable) 	// 使能TIMx在CCR1 PE_9  T1_CH1   PWMB ;
#define 	PWMC_EN   TIM_CCxNCmd(TIM1, TIM_Channel_1, TIM_CCxN_Enable)	// 使能TIMx在CCR1 PE_8  T1_CH1N  PWMC ;
#define 	PWMD_EN   TIM_CCxCmd(TIM1, TIM_Channel_2, TIM_CCx_Enable)		// 使能TIMx在CCR2 PE_11 T1_CH2   PWMD ;


void TIM1_PWM_Init(void);
void TIM2_12864_Init(void);
void TIM3_Cap_Init(void);
void TIM4_PWM_Init(void);
extern void TIM2_IRQHandler(void);
extern void TIM3_IRQHandler(void);
#endif

