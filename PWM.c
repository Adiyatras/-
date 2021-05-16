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
	 //第一步：配置时钟                 
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOB|RCC_APB2Periph_TIM1,ENABLE);
	 //第二步，配置goio口          
	  GPIO_InitStructure2.GPIO_Pin=GPIO_Pin_8|GPIO_Pin_11;         	//pin_8  pin_11  outC outD PWM反向
	 GPIO_InitStructure2.GPIO_Speed=GPIO_Speed_50MHz;         
	 GPIO_InitStructure2.GPIO_Mode=GPIO_Mode_AF_PP;                 //设置为复用浮空输出         
	 GPIO_Init(GPIOE,&GPIO_InitStructure2); 
	         
	 GPIO_InitStructure2.GPIO_Pin=GPIO_Pin_12|GPIO_Pin_9;          //pin_12  pin_9  outA outB
	 GPIO_InitStructure2.GPIO_Speed=GPIO_Speed_50MHz;         
	 GPIO_InitStructure2.GPIO_Mode=GPIO_Mode_AF_PP;                 //设置为复用浮空输出         
	 GPIO_Init(GPIOE,&GPIO_InitStructure2);

	 GPIO_PinRemapConfig(GPIO_FullRemap_TIM1, ENABLE);          //Timer1完全重映射  TIM1_CH1->PE9 
	 //第三步，定时器基本配置         
	 TIM_TimeBaseStructure.TIM_Period=1000-1;                   // 自动重装载寄存器的值        
	 TIM_TimeBaseStructure.TIM_Prescaler=72-1;                  // 时钟预分频数        
	 TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;      // 采样分频        
	 TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;  //向上计数        
	 TIM_TimeBaseStructure.TIM_RepetitionCounter=0;      //重复寄存器，用于自动更新pwm占空比                       
	 TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
	 //第四步pwm输出配置         
	 TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM2;                    //设置为pwm1输出模式         
	 TIM_OCInitStructure.TIM_Pulse=500;                                 //设置占空比时间         
	 TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_Low;             //设置输出极性         
	 TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;        //使能该通道输出 
	 
	 
	 //下面几个参数是高级定时器才会用到，通用定时器不用配置         
	 TIM_OCInitStructure.TIM_OCNPolarity=TIM_OCNPolarity_Low;        //设置互补端输出极性         
	 TIM_OCInitStructure.TIM_OutputNState=TIM_OutputNState_Enable;//使能互补端输出         
	 TIM_OCInitStructure.TIM_OCIdleState=TIM_OCIdleState_Reset;        //死区后输出状态         
	 TIM_OCInitStructure.TIM_OCNIdleState=TIM_OCNIdleState_Reset;//死区后互补端输出状态         
	 TIM_OC1Init(TIM1,&TIM_OCInitStructure);         //PE_8  T1_CH1N  PWMC ; PE_9 T1_CH1 PWMB ; OUTC OUTB PWM反向      //按照指定参数初始化           
	 TIM_OC2Init(TIM1,&TIM_OCInitStructure);   			 //PE_11 T1_CH2   PWMD ; outD  PWM正向
	 TIM_OC3Init(TIM1,&TIM_OCInitStructure);   			 //PE_12 T1_CH3N  PWMA ; outA  PWM正向
	 //第五步，死区和刹车功能配置，高级定时器才有的，通用定时器不用配置         
	 TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Disable;//运行模式下输出
	 TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Disable;//空闲模式下输出选择          
	 TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_OFF;         //锁定设置        
	 TIM_BDTRInitStructure.TIM_DeadTime = 0x90;                                         //死区时间设置         
	 TIM_BDTRInitStructure.TIM_Break = TIM_Break_Disable;                 //刹车功能使能         
	 TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High;//刹车输入极性        
	 TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;//自动输出使能          
	 TIM_BDTRConfig(TIM1,&TIM_BDTRInitStructure);
	 //第六步，使能端的打开 
	 TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);  //使能TIMx在CCR1上的预装载寄存器 
	 TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);  //使能TIMx在CCR1上的预装载寄存器 
	 TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);  //使能TIMx在CCR1上的预装载寄存器         
	 TIM_ARRPreloadConfig(TIM1, ENABLE);                //使能TIMx在ARR上的预装载寄存器         
	 TIM_Cmd(TIM1,ENABLE);                              //打开TIM2 
	 
				PWMA_DIS;	PWMB_DIS;	// 禁止使能 PWMA, PWMB ;
				PWMC_DIS;	PWMD_DIS;	// 禁止使能 PWMC, PWMD ;
			TIM_SetAutoreload(TIM1,1000);		// TIM1_ARR;
			
		// PWM部分;
		ISO_RESET_L;
		ISO_M1_L;
		ISO_M2_L;
		ISO_M3_L;
	 
	 //下面这句是高级定时器才有的，输出pwm必须打开         
	 TIM_CtrlPWMOutputs(TIM1, ENABLE);                                   //pwm输出使能，一定要记得打开 
}

void TIM2_12864_Init(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    /* 优先级组 说明了抢占优先级所用的位数，和响应优先级所用的位数   在这里是1， 3 
    0组：  抢占优先级占0位， 响应优先级占4位
    1组：  抢占优先级占1位， 响应优先级占3位
    2组：  抢占优先级占2位， 响应优先级占2位
    3组：  抢占优先级占3位， 响应优先级占1位
    4组:   抢占优先级占4位， 响应优先级占0位  */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE );
	
	TIM_TimeBaseStructure.TIM_Prescaler = 71;        			//72分频
	TIM_TimeBaseStructure.TIM_Period = 200;//311; //155;  				//自动重装寄存器周期值;0.312ms采集一次
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;				//时钟分割
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);				 //初始化定时器2
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;					 //定时器中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;		 //先占优先级0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;				 //从占优先级1
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);	
	
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);	 				//使能时钟TIM2定时中断源中断
	TIM_Cmd(TIM2, ENABLE);											//使能定时器2
}

void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update)!= RESET) 	// 检测中断是否发生;
	{
		LCD_Ying=(LCD_Ying+1)%2;
	}
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);	// 清除中断待处理位;
} 

void TIM3_Cap_Init(void)
{	 
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef  TIM3_ICInitStructure;
   	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	//使能TIM3时钟
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  //使能GPIOB时钟
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0|GPIO_Pin_1;  //PB0 清除之前设置  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PB0 输入  
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOB,GPIO_Pin_0|GPIO_Pin_1);						 //PB0 下拉
	
	//初始化定时器3 TIM3	 ,
	TIM_TimeBaseStructure.TIM_Period = 0XFFFF; //设定计数器自动重装值 
	TIM_TimeBaseStructure.TIM_Prescaler =72-1; 	//预分频器   
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
  
	//初始化TIM3输入捕获参数
	TIM3_ICInitStructure.TIM_Channel = TIM_Channel_3; //CC1S=01 	选择输入端 IC1映射到TI1上
  	TIM3_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//上升沿捕获
  	TIM3_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1上
  	TIM3_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //配置输入分频,不分频 
  	TIM3_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 配置输入滤波器 不滤波
  	TIM_ICInit(TIM3, &TIM3_ICInitStructure);
		
	TIM3_ICInitStructure.TIM_Channel = TIM_Channel_4; //CC1S=01 	选择输入端 IC1映射到TI1上
  	TIM3_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//上升沿捕获
  	TIM3_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1上
  	TIM3_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //配置输入分频,不分频 
  	TIM3_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 配置输入滤波器 不滤波
  	TIM_ICInit(TIM3, &TIM3_ICInitStructure);
	
	//中断分组初始化
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //先占优先级2级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //从优先级0级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器 
	
	TIM_ITConfig(TIM3,TIM_IT_CC3|TIM_IT_CC4,ENABLE);//允许更新中断 ,允许CC3IE捕获中断	TIM_IT_Update
	
   	TIM_Cmd(TIM3,ENABLE ); 	//使能定时器3
   
}

void TIM3_IRQHandler(void)
{ 
	if (TIM_GetITStatus(TIM3, TIM_IT_CC3) != RESET)//捕获3发生捕获事件
		{
		
			OUTX_Flag=1;
			if(MOT_DirX==1)				OUTX_Num++;
			else if(MOT_DirX==2)
			{
				if(OUTX_Num>0)	OUTX_Num--;
				else OUTX_Num=0;
			}
		}
		
	if (TIM_GetITStatus(TIM3, TIM_IT_CC4) != RESET)//捕获4发生捕获事件
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
					PWMA_DIS;	PWMB_DIS;	// 禁止使能 PWMA, PWMB ;X;
				}
				if(Run_Y == Start_Y)
				{
					PWMC_DIS;	PWMD_DIS;	// 禁止使能 PWMC, PWMD ;Y;
				}
				break;
			case 5:
				if(Run_X == PreDa_X)
				{
					PWMA_DIS;	PWMB_DIS;	// 禁止使能 PWMA, PWMB ;X;
				}
				if(Run_Y == PreDa_Y)
				{
					PWMC_DIS;	PWMD_DIS;	// 禁止使能 PWMC, PWMD ;Y;
				}
				break;
			case 6:
				if(Run_X == FollowDa_X)
				{
					PWMA_DIS;	PWMB_DIS;	// 禁止使能 PWMA, PWMB ;X;
				}
				if(Run_Y == FollowDa_Y)
				{
					PWMC_DIS;	PWMD_DIS;	// 禁止使能 PWMC, PWMD ;Y;
				}
				break;
			default: break;
		}

    TIM_ClearITPendingBit(TIM3, TIM_IT_CC3|TIM_IT_CC4); //清除中断标志位TIM_IT_Update
}

void TIM4_PWM_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
  TIM_BDTRInitTypeDef TIM4_BDTRInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);	//使能定时器3时钟
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB  | RCC_APB2Periph_AFIO, ENABLE);  //使能GPIO外设和AFIO复用功能模块时钟
	
	// JG: PB8,PB9, Z轴电机,舵机,;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9;	             
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;     //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);//   JG(2)
 
   //初始化TIM4
	TIM_TimeBaseStructure.TIM_Period = 1000-1; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler =72-1; //设置用来作为TIMx时钟频率除数的预分频值 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	
	//初始化TIM4 Channel3,4 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM4 OC3
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	TIM_OC4Init(TIM4, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM4 OC4
	
	TIM4_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
  TIM4_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
  TIM4_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_1;
  TIM4_BDTRInitStructure.TIM_DeadTime = 0x75;
  TIM4_BDTRInitStructure.TIM_Break = TIM_Break_Enable;
  TIM4_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High;
  TIM4_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
  TIM_BDTRConfig(TIM4,&TIM4_BDTRInitStructure);
 
  TIM_Cmd(TIM4,ENABLE);
	 //使能端的打开 
	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);  //使能TIMx在CCR3上的预装载寄存器 
	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);  //使能TIMx在CCR4上的预装载寄存器 
 
	TIM_ARRPreloadConfig(TIM4, ENABLE);                //使能TIMx在ARR上的预装载寄存器  
	TIM_Cmd(TIM4,ENABLE);                              //打开TIM4 
	 
	TIM_SetAutoreload(TIM4,20000);		// TIM4_ARR;
	TIM_SetCompare3(TIM4,MOT_Zpwm);	// TIM4_CRR3-Z;
	TIM_SetCompare4(TIM4,MOT_Spwm);	// TIM4_CRR4 机械手;
	 
	TIM_CCxCmd(TIM4, TIM_Channel_3, TIM_CCx_Enable); 		//使能TIMx在CCR3 PB8  T4_CH3 ;Z轴电机;
	TIM_CCxCmd(TIM4, TIM_Channel_4, TIM_CCx_Enable);  		//使能TIMx在CCR4 PB9  T4_CH4 ;舵机;
	// TIM_CCxCmd(TIM4, TIM_Channel_3, TIM_CCx_Disable); 		//禁止使能TIMx在CCR3 PB8  T4_CH3 ;Z轴电机;
	 
}


