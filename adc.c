
#include "stm32f10x.h"
#include "adc.h"
#include "math.h"
#include "stdio.h"

#define ADC1_DR_Address    ((u32)0x4001244C)
ADC_InitTypeDef ADC_InitStructure;
DMA_InitTypeDef DMA_InitStructure;

vu16 ADCConvertedValue[3];				// 自动存储ADC获得的数据;

unsigned long int AD_Data1=0;
unsigned long int AD_Data2=0;
unsigned long int AD_Data3=0;
unsigned long int AD_Value1=0;
unsigned long int AD_Value2=0;
unsigned long int AD_Value3=0;

extern unsigned char Set_flag;// 功能: 0=>主界面; 1=>区域设定; 2=>手动采摘; 3=>自动采摘;
extern unsigned long int AD_ResultX,AD_ResultY,AD_ResultV;
extern unsigned long int MOT_DirX,MOT_DirY,MOT_DirZ;// 电机方向: 0,停止; 1,右移,上移,前进; 2,左移,下移,后退;

extern unsigned int Run_Num,Run_V;// 数量, 压力;
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

	//PC0/1/3/4/5作为模拟通道输入引脚
	
	// JA: PC0,PC1, 摇杆电位器 PC0,IN10=>上下, PC1,IN11=>左右;
	// JA: PC3, 压力传感器 PC3,IN13=>压力;
	// JA: PC4,PC5, 电容板 PC4,IN14=>上下, PC5,IN15=>左右;
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;	             
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//模拟输入引脚
    GPIO_Init(GPIOC, &GPIO_InitStructure);
	
} 

void TIM5_Configuration(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE );
  	
	TIM_TimeBaseStructure.TIM_Prescaler = 71;        			//72分频
	TIM_TimeBaseStructure.TIM_Period = 155;//311; //155;  				//自动重装寄存器周期值;0.312ms采集一次
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;				//时钟分割
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//向上计数模式
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);				 //初始化定时器5
    TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);	 				//使能时钟TIM5定时中断源中断
	TIM_Cmd(TIM5, ENABLE);											//使能定时器5
}

void NVIC_Configuration(void)
{
	NVIC_InitTypeDef  NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);				 // 选择使用优先级分组第1组

	NVIC_InitStructure.NVIC_IRQChannel =DMA1_Channel1_IRQn;;        //ADC1_1_IRQn;    //          
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	 // 指定抢占式优先级别1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	

	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;					 //定时器中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;		 //先占优先级0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;				 //从占优先级1
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);	
}

void ADC_Configuration(void)
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);  
	
	NVIC_Configuration();		// DMA1_CH1,TIM2中断设置;
	/* GPIO configuration ------------------------------------------------------*/
	GPIO_Configuration();		// ADC输入GPIO初始化;
	TIM5_Configuration();		// 定时器TIM2初始化;
	/* DMA1 channel1 configuration ----------------------------------------------*/
	DMA_DeInit(DMA1_Channel1);

	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;			 //DMA对应的外设基地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADCConvertedValue;		 //内存存储基地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;					 //DMA的转换模式为SRC模式，由外设搬移到内存
	DMA_InitStructure.DMA_BufferSize = 3;								 //DMA缓存大小
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	 //接收一次数据后，设备地址禁止后移
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;				 //接收一次数据后，目标内存地址后移
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	 //定义外设数据宽度为16位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;			 //DMA搬移数据尺寸，HalfWord就是为16位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;						   //转换模式，循环缓存模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;					    //DMA优先级高
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;						   //M2M模式禁用
	DMA_Init(DMA1_Channel1, &DMA_InitStructure); 
	
	DMA_Cmd(DMA1_Channel1, ENABLE); // Enable DMA1 channel1 
	//DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);   		  //使能传输完成中断 ,在中断服务程序中读取转换值
	
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;				 //工作在独立模式
	ADC_InitStructure.ADC_ScanConvMode =ENABLE;      	//扫描模式,用于多通道采集
	ADC_InitStructure.ADC_ContinuousConvMode =ENABLE;                 //DISABLE;	   //连续转换模式
	ADC_InitStructure.ADC_ExternalTrigConv =ADC_ExternalTrigConv_None;               //不用外部触发转换，软件开启即可
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;			//数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 3;							//转换9个通道
	ADC_Init(ADC1, &ADC_InitStructure);
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div8); 					  //设置ad时钟为72/6=12M,ad最大时钟频率为14M
	
	//设定指定的ADC通道组,设置转化顺序和采样时间
	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 1, ADC_SampleTime_239Cycles5);    //UB1   19.95us采集一个点;
	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 2, ADC_SampleTime_239Cycles5);    //UA1   239.5周期,239.5/12M=19.95us;
	ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 3, ADC_SampleTime_239Cycles5);    //PC3,IN13  PC0,IN10=>上下, PC1,IN11=>左右;
	//ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 4, ADC_SampleTime_239Cycles5);    //PC4,IN14=>上下, PC5,IN15=>左右;
	
	ADC_ResetCalibration(ADC1);//使能ADC1复位校准寄存器
	while(ADC_GetResetCalibrationStatus(ADC1));	//获取ADC1重置校准寄存器的状态,设置状态则等待
    ADC_StartCalibration(ADC1); //开始指定ADC1的校准状态
	while(ADC_GetCalibrationStatus(ADC1));  //获取指定ADC1的校准程序,设置状态则等待   

    ADC_SoftwareStartConvCmd(ADC1, ENABLE);		 //由于没有采用外部触发，所以使用软件触发ADC转换
		
	DMA_ClearITPendingBit(DMA1_IT_TC1);  	// 清除DMA1中断待处理标志位;
	//DMA_Cmd(DMA1_Channel1, ENABLE); // Enable DMA1 channel1 
	DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);   		  //使能传输完成中断 ,在中断服务程序中读取转换值
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);		 //由于没有采用外部触发，所以使用软件触发ADC转换
	ADC_DMACmd(ADC1, DISABLE);	// 使能ADC的DMA1的请求;
	ADC_Cmd(ADC1, DISABLE);				// 禁止使能ADC;
	
	
}

void TIM5_IRQHandler(void)	 								
{   		 
	if (TIM_GetITStatus(TIM5, TIM_IT_Update)!= RESET) 	// 检测中断是否发生;
	{
	  ADC_DMACmd(ADC1, ENABLE);	// 使能ADC的DMA1的请求;
	  ADC_Cmd(ADC1, ENABLE);		// 使能ADC;
	}
	TIM_ClearITPendingBit(TIM5, TIM_IT_Update);	// 清除中断待处理位;
} 
						 
void  DMA1_Channel1_IRQHandler(void)  
{
	static  int m=0;
	//static  int i=0,m=0,ja=0,jb=0,jc=0;
	
	if(DMA_GetITStatus(DMA1_IT_TC1)!=RESET)	// DMA1通道是否发生中断
  {  
		AD_Value1=(unsigned int)ADCConvertedValue[0];// 外部数据向内存传数据，此时读取外部数据;
		AD_Value2=(unsigned int)ADCConvertedValue[1];
		AD_Value3=(unsigned int)ADCConvertedValue[2];
	DMA_ClearITPendingBit(DMA1_IT_TC1);  	// 清除DMA1中断待处理标志位;
	ADC_DMACmd(ADC1, DISABLE);		// 禁止使能ADC的DMA1的请求;
	ADC_Cmd(ADC1, DISABLE);				// 禁止使能ADC;
		
		TIM_ITConfig(TIM5, TIM_IT_Update, DISABLE);	 	// 禁止使能TIM3中断;
		TIM_Cmd(TIM5, DISABLE);		// 禁止使能TIM3;
		
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
		
		
		
		
		TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);	// 使能TIM3中断;
		TIM_Cmd(TIM5, ENABLE);		// 使能TIM3;
  }
}



