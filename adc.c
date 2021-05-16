
#include "stm32f10x.h"
#include "adc.h"
#include "math.h"
#include "stdio.h"

#define ADC1_DR_Address    ((u32)0x4001244C)
ADC_InitTypeDef ADC_InitStructure;
DMA_InitTypeDef DMA_InitStructure;

vu16 ADCConvertedValue[3];				// �Զ��洢ADC��õ�����;

unsigned long int AD_Data1=0;
unsigned long int AD_Data2=0;
unsigned long int AD_Data3=0;
unsigned long int AD_Value1=0;
unsigned long int AD_Value2=0;
unsigned long int AD_Value3=0;

extern unsigned char Set_flag;// ����: 0=>������; 1=>�����趨; 2=>�ֶ���ժ; 3=>�Զ���ժ;
extern unsigned long int AD_ResultX,AD_ResultY,AD_ResultV;
extern unsigned long int MOT_DirX,MOT_DirY,MOT_DirZ;// �������: 0,ֹͣ; 1,����,����,ǰ��; 2,����,����,����;

extern unsigned int Run_Num,Run_V;// ����, ѹ��;
extern unsigned long int AD_RunV_over;
extern unsigned int Apple_Pick;

void delay(u32 t)
{
	u32 i,j;
	for(i=0;i<t;i++)
		for(j=0;j<t;j++);
}

void GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	//PC0/1/3/4/5��Ϊģ��ͨ����������
	
	// JA: PC0,PC1, ҡ�˵�λ�� PC0,IN10=>����, PC1,IN11=>����;
	// JA: PC3, ѹ�������� PC3,IN13=>ѹ��;
	// JA: PC4,PC5, ���ݰ� PC4,IN14=>����, PC5,IN15=>����;
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;	             
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//ģ����������
    GPIO_Init(GPIOC, &GPIO_InitStructure);
	
} 

void TIM5_Configuration(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE );
  	
	TIM_TimeBaseStructure.TIM_Prescaler = 71;        			//72��Ƶ
	TIM_TimeBaseStructure.TIM_Period = 155;//311; //155;  				//�Զ���װ�Ĵ�������ֵ;0.312ms�ɼ�һ��
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;				//ʱ�ӷָ�
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//���ϼ���ģʽ
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);				 //��ʼ����ʱ��5
    TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);	 				//ʹ��ʱ��TIM5��ʱ�ж�Դ�ж�
	TIM_Cmd(TIM5, ENABLE);											//ʹ�ܶ�ʱ��5
}

void NVIC_Configuration(void)
{
	NVIC_InitTypeDef  NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);				 // ѡ��ʹ�����ȼ������1��

	NVIC_InitStructure.NVIC_IRQChannel =DMA1_Channel1_IRQn;;        //ADC1_1_IRQn;    //          
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	 // ָ����ռʽ���ȼ���1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	

	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;					 //��ʱ���ж�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;		 //��ռ���ȼ�0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;				 //��ռ���ȼ�1
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);	
}

void ADC_Configuration(void)
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);  
	
	NVIC_Configuration();		// DMA1_CH1,TIM2�ж�����;
	/* GPIO configuration ------------------------------------------------------*/
	GPIO_Configuration();		// ADC����GPIO��ʼ��;
	TIM5_Configuration();		// ��ʱ��TIM2��ʼ��;
	/* DMA1 channel1 configuration ----------------------------------------------*/
	DMA_DeInit(DMA1_Channel1);

	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;			 //DMA��Ӧ���������ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADCConvertedValue;		 //�ڴ�洢����ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;					 //DMA��ת��ģʽΪSRCģʽ����������Ƶ��ڴ�
	DMA_InitStructure.DMA_BufferSize = 3;								 //DMA�����С
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	 //����һ�����ݺ��豸��ַ��ֹ����
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;				 //����һ�����ݺ�Ŀ���ڴ��ַ����
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	 //�����������ݿ��Ϊ16λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;			 //DMA�������ݳߴ磬HalfWord����Ϊ16λ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;						   //ת��ģʽ��ѭ������ģʽ
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;					    //DMA���ȼ���
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;						   //M2Mģʽ����
	DMA_Init(DMA1_Channel1, &DMA_InitStructure); 
	
	DMA_Cmd(DMA1_Channel1, ENABLE); // Enable DMA1 channel1 
	//DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);   		  //ʹ�ܴ�������ж� ,���жϷ�������ж�ȡת��ֵ
	
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;				 //�����ڶ���ģʽ
	ADC_InitStructure.ADC_ScanConvMode =ENABLE;      	//ɨ��ģʽ,���ڶ�ͨ���ɼ�
	ADC_InitStructure.ADC_ContinuousConvMode =ENABLE;                 //DISABLE;	   //����ת��ģʽ
	ADC_InitStructure.ADC_ExternalTrigConv =ADC_ExternalTrigConv_None;               //�����ⲿ����ת���������������
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;			//�����Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = 3;							//ת��9��ͨ��
	ADC_Init(ADC1, &ADC_InitStructure);
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div8); 					  //����adʱ��Ϊ72/6=12M,ad���ʱ��Ƶ��Ϊ14M
	
	//�趨ָ����ADCͨ����,����ת��˳��Ͳ���ʱ��
	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 1, ADC_SampleTime_239Cycles5);    //UB1   19.95us�ɼ�һ����;
	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 2, ADC_SampleTime_239Cycles5);    //UA1   239.5����,239.5/12M=19.95us;
	ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 3, ADC_SampleTime_239Cycles5);    //PC3,IN13  PC0,IN10=>����, PC1,IN11=>����;
	//ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 4, ADC_SampleTime_239Cycles5);    //PC4,IN14=>����, PC5,IN15=>����;
	
	ADC_ResetCalibration(ADC1);//ʹ��ADC1��λУ׼�Ĵ���
	while(ADC_GetResetCalibrationStatus(ADC1));	//��ȡADC1����У׼�Ĵ�����״̬,����״̬��ȴ�
    ADC_StartCalibration(ADC1); //��ʼָ��ADC1��У׼״̬
	while(ADC_GetCalibrationStatus(ADC1));  //��ȡָ��ADC1��У׼����,����״̬��ȴ�   

    ADC_SoftwareStartConvCmd(ADC1, ENABLE);		 //����û�в����ⲿ����������ʹ���������ADCת��
		
	DMA_ClearITPendingBit(DMA1_IT_TC1);  	// ���DMA1�жϴ������־λ;
	//DMA_Cmd(DMA1_Channel1, ENABLE); // Enable DMA1 channel1 
	DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);   		  //ʹ�ܴ�������ж� ,���жϷ�������ж�ȡת��ֵ
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);		 //����û�в����ⲿ����������ʹ���������ADCת��
	ADC_DMACmd(ADC1, DISABLE);	// ʹ��ADC��DMA1������;
	ADC_Cmd(ADC1, DISABLE);				// ��ֹʹ��ADC;
	
	
}

void TIM5_IRQHandler(void)	 								
{   		 
	if (TIM_GetITStatus(TIM5, TIM_IT_Update)!= RESET) 	// ����ж��Ƿ���;
	{
	  ADC_DMACmd(ADC1, ENABLE);	// ʹ��ADC��DMA1������;
	  ADC_Cmd(ADC1, ENABLE);		// ʹ��ADC;
	}
	TIM_ClearITPendingBit(TIM5, TIM_IT_Update);	// ����жϴ�����λ;
} 
						 
void  DMA1_Channel1_IRQHandler(void)  
{
	static  int m=0;
	//static  int i=0,m=0,ja=0,jb=0,jc=0;
	
	if(DMA_GetITStatus(DMA1_IT_TC1)!=RESET)	// DMA1ͨ���Ƿ����ж�
  {  
		AD_Value1=(unsigned int)ADCConvertedValue[0];// �ⲿ�������ڴ洫���ݣ���ʱ��ȡ�ⲿ����;
		AD_Value2=(unsigned int)ADCConvertedValue[1];
		AD_Value3=(unsigned int)ADCConvertedValue[2];
	DMA_ClearITPendingBit(DMA1_IT_TC1);  	// ���DMA1�жϴ������־λ;
	ADC_DMACmd(ADC1, DISABLE);		// ��ֹʹ��ADC��DMA1������;
	ADC_Cmd(ADC1, DISABLE);				// ��ֹʹ��ADC;
		
		TIM_ITConfig(TIM5, TIM_IT_Update, DISABLE);	 	// ��ֹʹ��TIM3�ж�;
		TIM_Cmd(TIM5, DISABLE);		// ��ֹʹ��TIM3;
		
				AD_Data1 = AD_Value1&0x0FFF;
				AD_ResultX=AD_Data1*1.0/40.96*3.3;
				AD_Data2 = AD_Value2&0x0FFF;
				AD_ResultY=AD_Data2*1.0/40.96*3.3;
		
		AD_Data3 = AD_Data3+(AD_Value3&0x0FFF);
		m++;
		if(m>2)
		{
				AD_Data3 = AD_Data3/3;
				Run_V=AD_Data3*1.0/409.6*33;
			m=0;
		AD_Data3 = 0;
		}
		
		
		
		
		TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);	// ʹ��TIM3�ж�;
		TIM_Cmd(TIM5, ENABLE);		// ʹ��TIM3;
  }
}



