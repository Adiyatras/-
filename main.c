#include  "stm32f10x.h"
#include  "stm32f10x_conf.h" 
#include  "bsp.h"
#include  "adc.h"
#include  "PWM.h"
#include  "motor.h"
#include  "LCD12864.h"
#include  "keypad.h"
#include  "Adjust.h"
#include  "operate.h"

// �����򲿷�;
int main(void)
{
    BSP_Configuration();
		// ��ʾ����;
		GPIO_12864_initout();
		TIM2_12864_Init();
			// PWM����;
		TIM1_PWM_Init();
		TIM4_PWM_Init();
		TIM3_Cap_Init();
			// ADC ����;
		ADC_Configuration();
			// ����ͨ�Ų���;
		BSP_USART_Configuration(UART4);
		 
			if (SysTick_Config(72000))		    //ʱ�ӽ����ж�ʱ1msһ��  ���ڶ�ʱ 
			{
					while (1);
			}
		Flash_Init();// ��ȡX,Y������ͷƫ����;
		LCD_Init();
		Area_Init();// �����趨��ʼֵ;
		Alarm_Ctrl();
			
    while (1)// ��������;
    {
			Key_Event();
			if(key_val==16)
			{
				key_val=0;
				LCD_flag = 1;// ��ת������;
				Set_flag=0;// ϵͳ����;
				TIM_ITConfig(TIM2, TIM_IT_Update, DISABLE);	//��ֹʹ��ʱ��TIM2��ʱ�ж�Դ�ж�
				TIM_Cmd(TIM2, DISABLE);											//��ֹʹ�ܶ�ʱ��2
				break;
			}
			LCD_Display_Fresh();// Һ����ʾ;
		}
		while (1)// ������;
    {
			Key_Event();
			key_scan();
			switch(Set_flag)
			{
				case 0:		Set0_Home();// ������ ;
					break;
				case 1:		Set1_Area();// 1.�����趨 ;
					break;
				case 2:		Set2_Hand();// 2.�ֶ���ժ ;
					break;
				case 3:		Set3_Auto();// 3.�Զ���ժ ;
					break;
				case 4:		Set4_Adjust();// 4.У׼ ;
					break;
				default:break;
			}
			LCD_Display_Fresh();// Һ����ʾ;
			
		}
}


