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

// 主程序部分;
int main(void)
{
    BSP_Configuration();
		// 显示部分;
		GPIO_12864_initout();
		TIM2_12864_Init();
			// PWM部分;
		TIM1_PWM_Init();
		TIM4_PWM_Init();
		TIM3_Cap_Init();
			// ADC 部分;
		ADC_Configuration();
			// 串口通信部分;
		BSP_USART_Configuration(UART4);
		 
			if (SysTick_Config(72000))		    //时钟节拍中断时1ms一次  用于定时 
			{
					while (1);
			}
		Flash_Init();// 读取X,Y轴摄像头偏移量;
		LCD_Init();
		Area_Init();// 区域设定初始值;
		Alarm_Ctrl();
			
    while (1)// 开机界面;
    {
			Key_Event();
			if(key_val==16)
			{
				key_val=0;
				LCD_flag = 1;// 跳转主界面;
				Set_flag=0;// 系统启动;
				TIM_ITConfig(TIM2, TIM_IT_Update, DISABLE);	//禁止使能时钟TIM2定时中断源中断
				TIM_Cmd(TIM2, DISABLE);											//禁止使能定时器2
				break;
			}
			LCD_Display_Fresh();// 液晶显示;
		}
		while (1)// 主程序;
    {
			Key_Event();
			key_scan();
			switch(Set_flag)
			{
				case 0:		Set0_Home();// 主界面 ;
					break;
				case 1:		Set1_Area();// 1.区域设定 ;
					break;
				case 2:		Set2_Hand();// 2.手动采摘 ;
					break;
				case 3:		Set3_Auto();// 3.自动采摘 ;
					break;
				case 4:		Set4_Adjust();// 4.校准 ;
					break;
				default:break;
			}
			LCD_Display_Fresh();// 液晶显示;
			
		}
}


