/*
 * @file          bsp.c
 * @brief         bsp����
 * @ucos-ii       V2.86 
 * @function      DCP-28 
 * @compiler      RealView MDK-ARM Compiler V5.14
 * @peripheral    V3.5.0
 * @author  	  	
 * @date   	      2018/04/27
 * @corporation   �㽭���ǿƽ��豸���޹�˾
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
// ����ͨ�Ų���;
unsigned int RxCnt=0;
unsigned int RxDa[6];
unsigned int RxDa_X,RxDa_Y;// �Զ���ժʱ X,Y������ͷ�ɼ������;

/**********************************************************************************************************
* Function    ��void BSP_Configuration(void)
* Description : Ӳ����ʼ�� 
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
* Function    ��void BSP_GPIO_Configuration(void)
* Description : GPIO������;
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
    GPIO_Init(GPIOC, &GPIO_InitStructure);// ������;
    /*********JA~JI ���**************/   
	// JA: PC2, ҡ�˵�λ�� ����;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	             
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 			// ��������
    GPIO_Init(GPIOC, &GPIO_InitStructure);
	// JD: PD8-PD11, 204(P1) ������� H;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11;	             
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    //�������
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);// JD(4)
	// JD: PD12-PD15, 204(P1) ������� L;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;	             
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 			// ��������
    GPIO_Init(GPIOD, &GPIO_InitStructure);// JD(4)
	// JH: PE0-PE7, 218(P2) ���ʹ��;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;	             
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    //�������
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitStructure);// JH(8)
	// UART4--TX/RX,232����;
		GPIO_InitStructure.GPIO_Pin = BSP_IO_TXD4;	    						//UART4  TX232
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;            	//�����������
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);	// 232-TXD
    GPIO_InitStructure.GPIO_Pin = BSP_IO_RXD4;                 	// UART4	RX232
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;       // ���ÿ�©����
    GPIO_Init(GPIOC, &GPIO_InitStructure);	// 232-RXD
}


/**********************************************************************************************************
* Function    ��void BSP_EXTI_Configuration(void)
* Description : IO�ж�����
* Para        : none
* Return(s)   : none
* Example     : BSP_EXTI_Configuration();
**********************************************************************************************************/
void BSP_EXTI_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    //EXTI_InitTypeDef EXTI_InitStructure;
    /* Configure one bit for preemption priority */
    /* ���ȼ��� ˵������ռ���ȼ����õ�λ��������Ӧ���ȼ����õ�λ��   ��������1�� 3 
    0�飺  ��ռ���ȼ�ռ0λ�� ��Ӧ���ȼ�ռ4λ
    1�飺  ��ռ���ȼ�ռ1λ�� ��Ӧ���ȼ�ռ3λ
    2�飺  ��ռ���ȼ�ռ2λ�� ��Ӧ���ȼ�ռ2λ
    3�飺  ��ռ���ȼ�ռ3λ�� ��Ӧ���ȼ�ռ1λ
    4��:   ��ռ���ȼ�ռ4λ�� ��Ӧ���ȼ�ռ0λ  */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

    NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;			     //���ô���UART4�ж�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	     //��ռ���ȼ� 1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;               // �����ȼ�0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/**********************************************************************************************************
* Function    ��void BSP_USART_Configuration(USART_TypeDef* USARTx)
* Description : ����ͨ�Ų������� 
* Para        : none
* Return(s)   : none
* Example     : BSP_USART_Configuration(UART4);
**********************************************************************************************************/
void BSP_USART_Configuration(USART_TypeDef* USARTx)
{
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = 115200;									//����115200bps
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//����λ8λ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;				//ֹͣλ1λ
    USART_InitStructure.USART_Parity = USART_Parity_No;						//��У��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;   //��Ӳ������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;										//�շ�ģʽ
    /* Configure USART1 */
    USART_Init(USARTx, &USART_InitStructure);											//���ô��ڲ�������
    /* Enable USART1 Receive and Transmit interrupts */
    USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE);                //ʹ�ܽ����ж�
    USART_ITConfig(USARTx, USART_IT_TXE, ENABLE);									//ʹ�ܷ��ͻ�����ж�
    /* Enable the USART1 */
    USART_Cmd(USARTx, ENABLE);	
}

/**********************************************************************************************************
* Function    ��void BSP_USART_OUT(void)
* Description : ��Ƭ�����ڷ�������
* Para        : none
* Return(s)   : none
* Example     : BSP_USART_OUT(USART1, uint8_t *Data,uint16_t Len)
**********************************************************************************************************/
void BSP_USART_OUT(USART_TypeDef* USARTx, uint8_t *Data,uint16_t Len)
{ 
	uint16_t i;
	for(i=0; i<Len; i++)
	{
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC)==RESET);      //�ж��Ƿ�����ɡ�
        USART_SendData(USARTx, Data[i]);
	}
}

/**********************************************************************************************************
* Function    ��void UART4_IRQHandler(void)
* Description : ��Ƭ�����ڽ����ж� , ������ͷ�װ�ͨ��;
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
  		 RxTmp = USART_ReceiveData(UART4);   //�����Ĵ��������ݻ��浽���ջ�������
		 
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
									AuIma_ALL=0;// �ȴ��ж��Ƿ�������;//��ժǰλ������.
									if(Set_flag==3) AuRun_Flag=5;//�ع�ԭ��λ��;
								 }
								 else
								 {
									if(Set_flag==3) AuRun_Flag=4;//����ɨ��;
								 }
							 }
							 else if(RxTmp==0xFB)
							 {
								 RxCnt=0;
								 AuRun_Flag=0;
								 RDatNum=0;
								PWMA_DIS;	PWMB_DIS;	// ��ֹʹ�� PWMA, PWMB ;
								PWMC_DIS;	PWMD_DIS;	// ��ֹʹ�� PWMC, PWMD ;
								 
		while(USART_GetFlagStatus(UART4, USART_FLAG_TC)==RESET);      //�ж��Ƿ�����ɡ�
        USART_SendData(UART4, 0X81);	
		while(USART_GetFlagStatus(UART4, USART_FLAG_TC)==RESET);      //�ж��Ƿ�����ɡ�
        USART_SendData(UART4, 0X50);	
							 }
							 break;
						 case 2:						 	 RxCnt++;
									 AuIma_ALL=1;// ͼ��ȫ������;
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
									 FollowDa_X = Run_X-(80-shift_X)+RxDa_X*78/240;		//40//shift_X �Զ���ժʱ X,Y������ƫ����;
									 FollowDa_Y = Run_Y-(100-shift_Y)+RxDa_Y*104/320;		//50//shift_Y
									 if((FollowDa_X>Start_X) && (FollowDa_X<Stop_X) && (FollowDa_Y>Start_Y) && (FollowDa_Y<Stop_Y))
									 {
											PreDa_X=Run_X;
											PreDa_Y=Run_Y;
											Pre_MOT_X = MOT_FlagX;
											Pre_MOT_Y = MOT_FlagY;
											AuRun_Flag=6;// ����׷��;
											Auto_UART=0;// ֹͣͨѶ����;
		while(USART_GetFlagStatus(UART4, USART_FLAG_TC)==RESET);      //�ж��Ƿ�����ɡ�
        USART_SendData(UART4, 0X81);
		while(USART_GetFlagStatus(UART4, USART_FLAG_TC)==RESET);      //�ж��Ƿ�����ɡ�
        USART_SendData(UART4, 0X52);
									 }
									 else 
									 {
											PreDa_X=Start_X;	PreDa_Y=Start_Y;
											Pre_MOT_X = 0;		Pre_MOT_Y = 0;
											AuIma_ALL=0;// �ȴ��ж��Ƿ�������;//��ժǰλ������.
											Auto_UART=1;// ����ͨѶ����;
											if(Set_flag==3)AuRun_Flag=4;//����ɨ��;
										 
									 }
								 }
							 }
							 break;
						 default:break;
					 }
				 }
		 }
		 
	if(USART_GetITStatus(UART4, USART_IT_TXE) != RESET)                   //�����Ϊ�˱���STM32 USART ��һ���ֽڷ�����ȥ��BUG 
 	 { 
    	 USART_ITConfig(UART4, USART_IT_TXE, DISABLE);					     //��ֹ�����������жϣ� 
 	 }	
}



/*********************************************************************************************************
* Function    ��void BSP_RCC_Configuration(void)
* Description : ʱ�����ú���,ϵͳʱ������Ϊ72MHZ������ʱ������ 
* Para        : none
* Return(s)   : none
* Example     : BSP_RCC_Configuration();
*********************************************************************************************************/

void BSP_RCC_Configuration(void)
{
	SystemInit();
	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);// ����ӳ�䡢IO�жϽ��ж�дǰ��AFIO
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);// 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE, ENABLE);//  | RCC_APB2Periph_GPIOF
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
}


/*********************************************************************************************************
* Function    ��void Alarm_Ctrl(void)
* Description : ������ 
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
* Function    ��void key_scan(void)
* Description : ��������, ҡ�˵�λ������;
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
		if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_2 )==0)// ҡ�˵�λ������;
		{
			AN1_flag=1;
		}
			else AN1_flag=0;
}

/****************************************************************************
* ��    �ƣ�void Delay(__IO uint32_t nTime)
* ��    �ܣ���ʱ��ʱ���� 1msΪ��λ
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/  
void Delay(__IO uint32_t nTime)
{ 
  TimingDelay = nTime;

  while(TimingDelay != 0);
}
/****************************************************************************
* ��    �ƣ�void TimingDelay_Decrement(void)
* ��    �ܣ���ȡ���ĳ���
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/  
void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
  }
}



