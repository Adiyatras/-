/*
 * @file          bsp.c
 * @brief         bsp程序
 * @ucos-ii       V2.86 
 * @function      DCP-28 
 * @compiler      RealView MDK-ARM Compiler V5.14
 * @peripheral    V3.5.0
 * @author  	  	
 * @date   	      2018/04/27
 * @corporation   浙江求是科教设备有限公司
 * @website       http://www.zjqiushi.com/
*/
/* Includes ------------------------------------------------------------------*/
#include  "stm32f10x.h"
#include  "stm32f10x_conf.h" 
#include  "bsp.h"
#include  "LCD12864.h"
#include  "PWM.h"
#include  "motor.h"

/* Private variables ---------------------------------------------------------*/
// Systick delay
__IO uint32_t TimingDelay = 0 ;
// 串口通信部分;
unsigned int RxCnt=0;
unsigned int RxDa[6];
unsigned int RxDa_X,RxDa_Y;// 自动采摘时 X,Y轴摄像头采集坐标点;

/**********************************************************************************************************
* Function    ：void BSP_Configuration(void)
* Description : 硬件初始化 
* Para        : none
* Return(s)   : none
* Example     : BSP_Configuration();
**********************************************************************************************************/
void BSP_Configuration(void)
{
	BSP_RCC_Configuration();
    BSP_GPIO_Configuration(); 
	BSP_EXTI_Configuration();
}

/**********************************************************************************************************
* Function    ：void BSP_GPIO_Configuration(void)
* Description : GPIO口配置;
* Para        : none
* Return(s)   : none
* Example     : BSP_GPIO_Configuration();
**********************************************************************************************************/
void BSP_GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    /*********BUZZER**************/
    GPIO_InitStructure.GPIO_Pin   = BSP_GPIOC_BUZZER ;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);// 蜂鸣器;
    /*********JA~JI 输出**************/   
	// JA: PC2, 摇杆电位器 按键;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	             
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 			// 上拉输入
    GPIO_Init(GPIOC, &GPIO_InitStructure);
	// JD: PD8-PD11, 204(P1) 矩阵键盘 H;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11;	             
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    //推挽输出
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);// JD(4)
	// JD: PD12-PD15, 204(P1) 矩阵键盘 L;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;	             
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 			// 上拉输入
    GPIO_Init(GPIOD, &GPIO_InitStructure);// JD(4)
	// JH: PE0-PE7, 218(P2) 电机使能;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;	             
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    //推挽输出
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitStructure);// JH(8)
	// UART4--TX/RX,232串口;
		GPIO_InitStructure.GPIO_Pin = BSP_IO_TXD4;	    						//UART4  TX232
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;            	//复用推挽输出
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);	// 232-TXD
    GPIO_InitStructure.GPIO_Pin = BSP_IO_RXD4;                 	// UART4	RX232
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;       // 复用开漏输入
    GPIO_Init(GPIOC, &GPIO_InitStructure);	// 232-RXD
}


/**********************************************************************************************************
* Function    ：void BSP_EXTI_Configuration(void)
* Description : IO中断配置
* Para        : none
* Return(s)   : none
* Example     : BSP_EXTI_Configuration();
**********************************************************************************************************/
void BSP_EXTI_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    //EXTI_InitTypeDef EXTI_InitStructure;
    /* Configure one bit for preemption priority */
    /* 优先级组 说明了抢占优先级所用的位数，和响应优先级所用的位数   在这里是1， 3 
    0组：  抢占优先级占0位， 响应优先级占4位
    1组：  抢占优先级占1位， 响应优先级占3位
    2组：  抢占优先级占2位， 响应优先级占2位
    3组：  抢占优先级占3位， 响应优先级占1位
    4组:   抢占优先级占4位， 响应优先级占0位  */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

    NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;			     //设置串口UART4中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	     //抢占优先级 1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;               // 子优先级0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/**********************************************************************************************************
* Function    ：void BSP_USART_Configuration(USART_TypeDef* USARTx)
* Description : 串口通信参数设置 
* Para        : none
* Return(s)   : none
* Example     : BSP_USART_Configuration(UART4);
**********************************************************************************************************/
void BSP_USART_Configuration(USART_TypeDef* USARTx)
{
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = 115200;									//速率115200bps
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//数据位8位
    USART_InitStructure.USART_StopBits = USART_StopBits_1;				//停止位1位
    USART_InitStructure.USART_Parity = USART_Parity_No;						//无校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;   //无硬件流控
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;										//收发模式
    /* Configure USART1 */
    USART_Init(USARTx, &USART_InitStructure);											//配置串口参数函数
    /* Enable USART1 Receive and Transmit interrupts */
    USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE);                //使能接收中断
    USART_ITConfig(USARTx, USART_IT_TXE, ENABLE);									//使能发送缓冲空中断
    /* Enable the USART1 */
    USART_Cmd(USARTx, ENABLE);	
}

/**********************************************************************************************************
* Function    ：void BSP_USART_OUT(void)
* Description : 单片机串口发送数据
* Para        : none
* Return(s)   : none
* Example     : BSP_USART_OUT(USART1, uint8_t *Data,uint16_t Len)
**********************************************************************************************************/
void BSP_USART_OUT(USART_TypeDef* USARTx, uint8_t *Data,uint16_t Len)
{ 
	uint16_t i;
	for(i=0; i<Len; i++)
	{
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC)==RESET);      //判断是否发送完成。
        USART_SendData(USARTx, Data[i]);
	}
}

/**********************************************************************************************************
* Function    ：void UART4_IRQHandler(void)
* Description : 单片机串口接收中断 , 与摄像头底板通信;
* Para        : none
* Return(s)   : none
* Example     : UART4_IRQHandler();
**********************************************************************************************************/
void UART4_IRQHandler(void)
{
 	static unsigned char RDatNum;
 	unsigned char RxTmp=0;
   	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)
 	 {
  		 RxTmp = USART_ReceiveData(UART4);   //将读寄存器的数据缓存到接收缓冲区里
		 
		 if(Auto_UART == 1)//
		 {
					 switch(RxCnt)
					 {
						 case 0:	if(RxTmp==0x80) RxCnt=1;
							 break;
						 case 1:
							 if(RxTmp==0xFE) RxCnt++;
							 else if(RxTmp==0xFA)
							 {
								 RxCnt=0;
								 RDatNum=0;
								 if(AuIma_ALL!=0)
								 {
									AuIma_ALL=0;// 等待判定是否在屏内;//采摘前位置清零.
									if(Set_flag==3) AuRun_Flag=5;//回归原来位置;
								 }
								 else
								 {
									if(Set_flag==3) AuRun_Flag=4;//继续扫描;
								 }
							 }
							 else if(RxTmp==0xFB)
							 {
								 RxCnt=0;
								 AuRun_Flag=0;
								 RDatNum=0;
								PWMA_DIS;	PWMB_DIS;	// 禁止使能 PWMA, PWMB ;
								PWMC_DIS;	PWMD_DIS;	// 禁止使能 PWMC, PWMD ;
								 
		while(USART_GetFlagStatus(UART4, USART_FLAG_TC)==RESET);      //判断是否发送完成。
        USART_SendData(UART4, 0X81);	
		while(USART_GetFlagStatus(UART4, USART_FLAG_TC)==RESET);      //判断是否发送完成。
        USART_SendData(UART4, 0X50);	
							 }
							 break;
						 case 2:						 	 RxCnt++;
									 AuIma_ALL=1;// 图像全在屏内;
							 break;
						 case 3:
							 RxDa[0] = RxTmp;		 RxCnt++;
							 break;
						 case 4:
							 RxDa[1] = RxTmp;		 RxCnt++;
							 break;
						 case 5:
							 RxDa[2] = RxTmp;		 RxCnt++;
							 break;
						 case 6:
							 RxDa[3] = RxTmp;		 RxCnt++;
							 break;
						 case 7:
							 RxDa[4] = RxTmp;		 RxCnt++;
							 break;
						 case 8:
							 RxDa[5] = RxTmp;		 RxCnt++;
							 break;
						 case 9:
							 RxCnt=0;
							 if(RxTmp==0xfe)
							 {
								 RxDa_X = RxDa[0]*100+RxDa[1]*10+RxDa[2];
								 RxDa_Y = RxDa[3]*100+RxDa[4]*10+RxDa[5];
									 RDatNum++;
								 if(RDatNum>0) 
								 {
									 FollowDa_X = Run_X-(80-shift_X)+RxDa_X*78/240;		//40//shift_X 自动采摘时 X,Y轴坐标偏移量;
									 FollowDa_Y = Run_Y-(100-shift_Y)+RxDa_Y*104/320;		//50//shift_Y
									 if((FollowDa_X>Start_X) && (FollowDa_X<Stop_X) && (FollowDa_Y>Start_Y) && (FollowDa_Y<Stop_Y))
									 {
											PreDa_X=Run_X;
											PreDa_Y=Run_Y;
											Pre_MOT_X = MOT_FlagX;
											Pre_MOT_Y = MOT_FlagY;
											AuRun_Flag=6;// 启动追踪;
											Auto_UART=0;// 停止通讯接收;
		while(USART_GetFlagStatus(UART4, USART_FLAG_TC)==RESET);      //判断是否发送完成。
        USART_SendData(UART4, 0X81);
		while(USART_GetFlagStatus(UART4, USART_FLAG_TC)==RESET);      //判断是否发送完成。
        USART_SendData(UART4, 0X52);
									 }
									 else 
									 {
											PreDa_X=Start_X;	PreDa_Y=Start_Y;
											Pre_MOT_X = 0;		Pre_MOT_Y = 0;
											AuIma_ALL=0;// 等待判定是否在屏内;//采摘前位置清零.
											Auto_UART=1;// 启动通讯接收;
											if(Set_flag==3)AuRun_Flag=4;//继续扫描;
										 
									 }
								 }
							 }
							 break;
						 default:break;
					 }
				 }
		 }
		 
	if(USART_GetITStatus(UART4, USART_IT_TXE) != RESET)                   //这段是为了避免STM32 USART 第一个字节发不出去的BUG 
 	 { 
    	 USART_ITConfig(UART4, USART_IT_TXE, DISABLE);					     //禁止发缓冲器空中断， 
 	 }	
}



/*********************************************************************************************************
* Function    ：void BSP_RCC_Configuration(void)
* Description : 时钟配置函数,系统时钟配置为72MHZ，外设时钟配置 
* Para        : none
* Return(s)   : none
* Example     : BSP_RCC_Configuration();
*********************************************************************************************************/

void BSP_RCC_Configuration(void)
{
	SystemInit();
	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);// 引脚映射、IO中断进行读写前打开AFIO
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);// 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE, ENABLE);//  | RCC_APB2Periph_GPIOF
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
}


/*********************************************************************************************************
* Function    ：void Alarm_Ctrl(void)
* Description : 蜂鸣器 
* Para        : none
* Return(s)   : none
* Example     : Alarm_Ctrl();
*********************************************************************************************************/
void Alarm_Ctrl(void)
{
	GPIO_SetBits(GPIOC, BSP_GPIOC_BUZZER);
	Delay(300);
    GPIO_ResetBits(GPIOC, BSP_GPIOC_BUZZER);
}


/*********************************************************************************************************
* Function    ：void key_scan(void)
* Description : 独立按键, 摇杆电位器按键;
* Para        : none
* Return(s)   : none
* Example     : key_scan();
*********************************************************************************************************/
void key_scan(void)
{
		if(GPIO_ReadInputDataBit(GPIOG, BSP_GPIOG_SW1 )==0)// SW1
		{
			press_flag1=1;
		}
			else press_flag1=0;
		if(GPIO_ReadInputDataBit(GPIOG, BSP_GPIOG_SW2 )==0)// SW2
		{
			press_flag2=1;
		}
			else press_flag2=0;
		if(GPIO_ReadInputDataBit(GPIOG, BSP_GPIOG_SW3 )==0)// SW3
		{
			press_flag3=1;
		}
			else press_flag3=0;
		if(GPIO_ReadInputDataBit(GPIOG, BSP_GPIOG_SW4 )==0)// SW4
		{
			press_flag4=1;
		}
			else press_flag4=0;
		if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_2 )==0)// 摇杆电位器按键;
		{
			AN1_flag=1;
		}
			else AN1_flag=0;
}

/****************************************************************************
* 名    称：void Delay(__IO uint32_t nTime)
* 功    能：定时延时程序 1ms为单位
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/  
void Delay(__IO uint32_t nTime)
{ 
  TimingDelay = nTime;

  while(TimingDelay != 0);
}
/****************************************************************************
* 名    称：void TimingDelay_Decrement(void)
* 功    能：获取节拍程序
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/  
void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
  }
}



