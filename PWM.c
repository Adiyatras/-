#include  "stm32f10x.h"
#include  "stm32f10x_conf.h" 
#include  "bsp.h"
#include  "PWM.h"
#include  "keypad.h"
#include  "motor.h"

//#define START          0X01  
//#define WAIT_RISING    0X02  
//#define WAIT_FALLING   0X03  

u16 OUTX_Num=0;
u16 OUTY_Num=0;
u16 OUTX_Flag=0;
u16 OUTY_Flag=0;

void TIM1_PWM_Init(void)
{
     GPIO_InitTypeDef GPIO_InitStructure2;         
	 TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;        
	 TIM_OCInitTypeDef TIM_OCInitStructure;        
	 TIM_BDTRInitTypeDef TIM_BDTRInitStructure;
	 //��һ��������ʱ��                 
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOB|RCC_APB2Periph_TIM1,ENABLE);
	 //�ڶ���������goio��          
	  GPIO_InitStructure2.GPIO_Pin=GPIO_Pin_8|GPIO_Pin_11;         	//pin_8  pin_11  outC outD PWM����
	 GPIO_InitStructure2.GPIO_Speed=GPIO_Speed_50MHz;         
	 GPIO_InitStructure2.GPIO_Mode=GPIO_Mode_AF_PP;                 //����Ϊ���ø������         
	 GPIO_Init(GPIOE,&GPIO_InitStructure2); 
	         
	 GPIO_InitStructure2.GPIO_Pin=GPIO_Pin_12|GPIO_Pin_9;          //pin_12  pin_9  outA outB
	 GPIO_InitStructure2.GPIO_Speed=GPIO_Speed_50MHz;         
	 GPIO_InitStructure2.GPIO_Mode=GPIO_Mode_AF_PP;                 //����Ϊ���ø������         
	 GPIO_Init(GPIOE,&GPIO_InitStructure2);

	 GPIO_PinRemapConfig(GPIO_FullRemap_TIM1, ENABLE);          //Timer1��ȫ��ӳ��  TIM1_CH1->PE9 
	 //����������ʱ����������         
	 TIM_TimeBaseStructure.TIM_Period=1000-1;                   // �Զ���װ�ؼĴ�����ֵ        
	 TIM_TimeBaseStructure.TIM_Prescaler=72-1;                  // ʱ��Ԥ��Ƶ��        
	 TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;      // ������Ƶ        
	 TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;  //���ϼ���        
	 TIM_TimeBaseStructure.TIM_RepetitionCounter=0;      //�ظ��Ĵ����������Զ�����pwmռ�ձ�                       
	 TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
	 //���Ĳ�pwm�������         
	 TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM2;                    //����Ϊpwm1���ģʽ         
	 TIM_OCInitStructure.TIM_Pulse=500;                                 //����ռ�ձ�ʱ��         
	 TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_Low;             //�����������         
	 TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;        //ʹ�ܸ�ͨ����� 
	 
	 
	 //���漸�������Ǹ߼���ʱ���Ż��õ���ͨ�ö�ʱ����������         
	 TIM_OCInitStructure.TIM_OCNPolarity=TIM_OCNPolarity_Low;        //���û������������         
	 TIM_OCInitStructure.TIM_OutputNState=TIM_OutputNState_Enable;//ʹ�ܻ��������         
	 TIM_OCInitStructure.TIM_OCIdleState=TIM_OCIdleState_Reset;        //���������״̬         
	 TIM_OCInitStructure.TIM_OCNIdleState=TIM_OCNIdleState_Reset;//�����󻥲������״̬         
	 TIM_OC1Init(TIM1,&TIM_OCInitStructure);         //PE_8  T1_CH1N  PWMC ; PE_9 T1_CH1 PWMB ; OUTC OUTB PWM����      //����ָ��������ʼ��           
	 TIM_OC2Init(TIM1,&TIM_OCInitStructure);   			 //PE_11 T1_CH2   PWMD ; outD  PWM����
	 TIM_OC3Init(TIM1,&TIM_OCInitStructure);   			 //PE_12 T1_CH3N  PWMA ; outA  PWM����
	 //���岽��������ɲ���������ã��߼���ʱ�����еģ�ͨ�ö�ʱ����������         
	 TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Disable;//����ģʽ�����
	 TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Disable;//����ģʽ�����ѡ��          
	 TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_OFF;         //��������        
	 TIM_BDTRInitStructure.TIM_DeadTime = 0x90;                                         //����ʱ������         
	 TIM_BDTRInitStructure.TIM_Break = TIM_Break_Disable;                 //ɲ������ʹ��         
	 TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High;//ɲ�����뼫��        
	 TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;//�Զ����ʹ��          
	 TIM_BDTRConfig(TIM1,&TIM_BDTRInitStructure);
	 //��������ʹ�ܶ˵Ĵ� 
	 TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);  //ʹ��TIMx��CCR1�ϵ�Ԥװ�ؼĴ��� 
	 TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);  //ʹ��TIMx��CCR1�ϵ�Ԥװ�ؼĴ��� 
	 TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);  //ʹ��TIMx��CCR1�ϵ�Ԥװ�ؼĴ���         
	 TIM_ARRPreloadConfig(TIM1, ENABLE);                //ʹ��TIMx��ARR�ϵ�Ԥװ�ؼĴ���         
	 TIM_Cmd(TIM1,ENABLE);                              //��TIM2 
	 
				PWMA_DIS;	PWMB_DIS;	// ��ֹʹ�� PWMA, PWMB ;
				PWMC_DIS;	PWMD_DIS;	// ��ֹʹ�� PWMC, PWMD ;
			TIM_SetAutoreload(TIM1,1000);		// TIM1_ARR;
			
		// PWM����;
		ISO_RESET_L;
		ISO_M1_L;
		ISO_M2_L;
		ISO_M3_L;
	 
	 //��������Ǹ߼���ʱ�����еģ����pwm�����         
	 TIM_CtrlPWMOutputs(TIM1, ENABLE);                                   //pwm���ʹ�ܣ�һ��Ҫ�ǵô� 
}

void TIM2_12864_Init(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    /* ���ȼ��� ˵������ռ���ȼ����õ�λ��������Ӧ���ȼ����õ�λ��   ��������1�� 3 
    0�飺  ��ռ���ȼ�ռ0λ�� ��Ӧ���ȼ�ռ4λ
    1�飺  ��ռ���ȼ�ռ1λ�� ��Ӧ���ȼ�ռ3λ
    2�飺  ��ռ���ȼ�ռ2λ�� ��Ӧ���ȼ�ռ2λ
    3�飺  ��ռ���ȼ�ռ3λ�� ��Ӧ���ȼ�ռ1λ
    4��:   ��ռ���ȼ�ռ4λ�� ��Ӧ���ȼ�ռ0λ  */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE );
	
	TIM_TimeBaseStructure.TIM_Prescaler = 71;        			//72��Ƶ
	TIM_TimeBaseStructure.TIM_Period = 200;//311; //155;  				//�Զ���װ�Ĵ�������ֵ;0.312ms�ɼ�һ��
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;				//ʱ�ӷָ�
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//���ϼ���ģʽ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);				 //��ʼ����ʱ��2
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;					 //��ʱ���ж�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;		 //��ռ���ȼ�0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;				 //��ռ���ȼ�1
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);	
	
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);	 				//ʹ��ʱ��TIM2��ʱ�ж�Դ�ж�
	TIM_Cmd(TIM2, ENABLE);											//ʹ�ܶ�ʱ��2
}

void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update)!= RESET) 	// ����ж��Ƿ���;
	{
		LCD_Ying=(LCD_Ying+1)%2;
	}
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);	// ����жϴ�����λ;
} 

void TIM3_Cap_Init(void)
{	 
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef  TIM3_ICInitStructure;
   	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	//ʹ��TIM3ʱ��
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  //ʹ��GPIOBʱ��
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0|GPIO_Pin_1;  //PB0 ���֮ǰ����  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PB0 ����  
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOB,GPIO_Pin_0|GPIO_Pin_1);						 //PB0 ����
	
	//��ʼ����ʱ��3 TIM3	 ,
	TIM_TimeBaseStructure.TIM_Period = 0XFFFF; //�趨�������Զ���װֵ 
	TIM_TimeBaseStructure.TIM_Prescaler =72-1; 	//Ԥ��Ƶ��   
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
  
	//��ʼ��TIM3���벶�����
	TIM3_ICInitStructure.TIM_Channel = TIM_Channel_3; //CC1S=01 	ѡ������� IC1ӳ�䵽TI1��
  	TIM3_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//�����ز���
  	TIM3_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //ӳ�䵽TI1��
  	TIM3_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //���������Ƶ,����Ƶ 
  	TIM3_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 ���������˲��� ���˲�
  	TIM_ICInit(TIM3, &TIM3_ICInitStructure);
		
	TIM3_ICInitStructure.TIM_Channel = TIM_Channel_4; //CC1S=01 	ѡ������� IC1ӳ�䵽TI1��
  	TIM3_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//�����ز���
  	TIM3_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //ӳ�䵽TI1��
  	TIM3_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //���������Ƶ,����Ƶ 
  	TIM3_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 ���������˲��� ���˲�
  	TIM_ICInit(TIM3, &TIM3_ICInitStructure);
	
	//�жϷ����ʼ��
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //��ռ���ȼ�2��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //�����ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ��� 
	
	TIM_ITConfig(TIM3,TIM_IT_CC3|TIM_IT_CC4,ENABLE);//��������ж� ,����CC3IE�����ж�	TIM_IT_Update
	
   	TIM_Cmd(TIM3,ENABLE ); 	//ʹ�ܶ�ʱ��3
   
}

void TIM3_IRQHandler(void)
{ 
	if (TIM_GetITStatus(TIM3, TIM_IT_CC3) != RESET)//����3���������¼�
		{
		
			OUTX_Flag=1;
			if(MOT_DirX==1)				OUTX_Num++;
			else if(MOT_DirX==2)
			{
				if(OUTX_Num>0)	OUTX_Num--;
				else OUTX_Num=0;
			}
		}
		
	if (TIM_GetITStatus(TIM3, TIM_IT_CC4) != RESET)//����4���������¼�
		{
			OUTY_Flag=1;
			if(MOT_DirY==1)				OUTY_Num++;
			else if(MOT_DirY==2)
			{
				if(OUTY_Num>0)	OUTY_Num--;
				else OUTY_Num=0;
			}
		}
			Run_X = (OUTX_Num*4)/330;
			Run_Y = (OUTY_Num*8)/330;
		
		switch(AuRun_Flag)
		{
			case 3:
				if(Run_X == Start_X)
				{
					PWMA_DIS;	PWMB_DIS;	// ��ֹʹ�� PWMA, PWMB ;X;
				}
				if(Run_Y == Start_Y)
				{
					PWMC_DIS;	PWMD_DIS;	// ��ֹʹ�� PWMC, PWMD ;Y;
				}
				break;
			case 5:
				if(Run_X == PreDa_X)
				{
					PWMA_DIS;	PWMB_DIS;	// ��ֹʹ�� PWMA, PWMB ;X;
				}
				if(Run_Y == PreDa_Y)
				{
					PWMC_DIS;	PWMD_DIS;	// ��ֹʹ�� PWMC, PWMD ;Y;
				}
				break;
			case 6:
				if(Run_X == FollowDa_X)
				{
					PWMA_DIS;	PWMB_DIS;	// ��ֹʹ�� PWMA, PWMB ;X;
				}
				if(Run_Y == FollowDa_Y)
				{
					PWMC_DIS;	PWMD_DIS;	// ��ֹʹ�� PWMC, PWMD ;Y;
				}
				break;
			default: break;
		}

    TIM_ClearITPendingBit(TIM3, TIM_IT_CC3|TIM_IT_CC4); //����жϱ�־λTIM_IT_Update
}

void TIM4_PWM_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
  TIM_BDTRInitTypeDef TIM4_BDTRInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);	//ʹ�ܶ�ʱ��3ʱ��
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB  | RCC_APB2Periph_AFIO, ENABLE);  //ʹ��GPIO�����AFIO���ù���ģ��ʱ��
	
	// JG: PB8,PB9, Z����,���,;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9;	             
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;     //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);//   JG(2)
 
   //��ʼ��TIM4
	TIM_TimeBaseStructure.TIM_Period = 1000-1; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler =72-1; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	
	//��ʼ��TIM4 Channel3,4 PWMģʽ	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM4 OC3
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�
	TIM_OC4Init(TIM4, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM4 OC4
	
	TIM4_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
  TIM4_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
  TIM4_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_1;
  TIM4_BDTRInitStructure.TIM_DeadTime = 0x75;
  TIM4_BDTRInitStructure.TIM_Break = TIM_Break_Enable;
  TIM4_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High;
  TIM4_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
  TIM_BDTRConfig(TIM4,&TIM4_BDTRInitStructure);
 
  TIM_Cmd(TIM4,ENABLE);
	 //ʹ�ܶ˵Ĵ� 
	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);  //ʹ��TIMx��CCR3�ϵ�Ԥװ�ؼĴ��� 
	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);  //ʹ��TIMx��CCR4�ϵ�Ԥװ�ؼĴ��� 
 
	TIM_ARRPreloadConfig(TIM4, ENABLE);                //ʹ��TIMx��ARR�ϵ�Ԥװ�ؼĴ���  
	TIM_Cmd(TIM4,ENABLE);                              //��TIM4 
	 
	TIM_SetAutoreload(TIM4,20000);		// TIM4_ARR;
	TIM_SetCompare3(TIM4,MOT_Zpwm);	// TIM4_CRR3-Z;
	TIM_SetCompare4(TIM4,MOT_Spwm);	// TIM4_CRR4 ��е��;
	 
	TIM_CCxCmd(TIM4, TIM_Channel_3, TIM_CCx_Enable); 		//ʹ��TIMx��CCR3 PB8  T4_CH3 ;Z����;
	TIM_CCxCmd(TIM4, TIM_Channel_4, TIM_CCx_Enable);  		//ʹ��TIMx��CCR4 PB9  T4_CH4 ;���;
	// TIM_CCxCmd(TIM4, TIM_Channel_3, TIM_CCx_Disable); 		//��ֹʹ��TIMx��CCR3 PB8  T4_CH3 ;Z����;
	 
}


