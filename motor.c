#include "stm32f10x.h"
#include  "stm32f10x_conf.h" 
#include  "bsp.h"
#include  "adc.h"
#include  "PWM.h"
#include  "motor.h"
#include  "LCD12864.h"
#include  "keypad.h"
#include  "operate.h"

// 按键部分;
unsigned char press_flag1=0;// 独立按键1;
unsigned char press_flag2=0;// 独立按键2;
unsigned char press_flag3=0;// 独立按键3;
unsigned char press_flag4=0;// 独立按键4;
unsigned char	key_Pressed,key_flag;
unsigned char	key_val=0;// 矩阵按键键值;
// 界面显示部分;
unsigned char LCD_flag=0;// 界面号: 0=>LCD_Start; 1=>LCD_Main; 2=>LCD_Set; 3=>LCD_State; 4=>坐标点图;
unsigned char LCD_Ying=0;// LCD闪烁标志;
unsigned char Set_flag=0;// 功能: 0=>主界面; 1=>区域设定; 2=>手动采摘; 3=>自动采摘;
unsigned int Start_X,Start_Y,Stop_X,Stop_Y,Run_X,Run_Y;// 起点坐标, 终点坐标; 机械手坐标;
unsigned int Run_Num,Run_V;// 数量, 压力;
unsigned int Set_Data[12];// 区域设定界面参数设定值;
unsigned char Set_Wei=0;// 区域设定界面参数位;
// 上电归零位部分;
unsigned int  Auto_end=0;// 自动运行标志:0-正常自动运行;1-自动运行结束动作;2-自动运行已结束(等待退出界面);
unsigned int  Grid_init=0;// 归零位标志:1-已完成上电归零位操作,无需再做;
unsigned int  Grid0_Num=0;// 归零位操作:识别脉冲有无延时;
// ADC 部分;
unsigned char AN1_flag=0;// 摇杆按键;
unsigned long int AD_ResultX=150;// 摇杆ADC3;
unsigned long int AD_ResultY=150;// 摇杆ADC2;
unsigned long int AD_ResultV=150;// 压力;
unsigned long int AD_RunV_over=0;// 超压力标志;
// 采摘控制部分;
unsigned int  AuRun_Flag=0;// 电机动作:1-归零位(上电仅一次);2-手动操作;3-归区域零点;4-自动运行;5-归追踪前位置;6-追踪目标;
unsigned int  Auto_UART=0;// 1-开启通信接收;0-禁止通信接收;
unsigned int  AuIma_ALL=0;// 启动自动扫描及回到发现点原点时归0,全屏为 1 ,半屏为 2 ;
unsigned int  Apple_Pick=0;// 水果采摘动作分级;0-无动作;1-开始动作;2,3,4,....直至动作结束归0;
unsigned int  Pick_time1,Pick_time2;// 手动采摘机械手前进,后退计时;
unsigned int  Pick_line,Pick_Htim;//自动运行时的行数,列上升计时;
unsigned int  PreDa_X,PreDa_Y;// 采摘追踪前坐标;
unsigned int  FollowDa_X,FollowDa_Y;// 被追踪目标坐标;
unsigned int  Pick_DaX[Pick_NumMax],Pick_DaY[Pick_NumMax];// 采摘水果动作坐标;
// 采摘参数可设置变量;
unsigned int  MOT_Spwm_MAX=1600; 				// 	机械手舵机伸缩最大间距;max;1500~1750可调;
unsigned int  MOT_Spwm_MIN=1400; 				// 	机械手舵机伸缩最小间距;min;1250~1500可调;
unsigned int  Pick_time_M=14;						// 	Z轴出去或回来最大延时;
unsigned int  shift_X=40;								// 自动采摘时 X,Y轴摄像头偏移量;X:0~80可调;Y:0~100可调;
unsigned int  shift_Y=50;								// 自动采摘时 X,Y轴摄像头偏移量;X:0~80可调;Y:0~100可调;
// 电机部分;
unsigned int MOT_DirX,MOT_DirY,MOT_DirZ;// X,Y,Z.电机方向: 0,停止; 1,右移,上移,前进; 2,左移,下移,后退;,MOT_DirV
unsigned int MOT_FlagX=0;// X轴电机动作标志;
unsigned int MOT_FlagY=0;// Y轴电机动作标志;
unsigned int Pre_MOT_X,Pre_MOT_Y;// 自动运行:采摘追踪前X Y轴电机动作标志;
// PWM部分;
unsigned int OUTA_pwm=500;// A,B,C,D.PWM;
unsigned int OUTB_pwm=500;
unsigned int OUTC_pwm=500;
unsigned int OUTD_pwm=500;
unsigned int MOT_Zpwm=10000;// Z轴电机PWM;
unsigned int MOT_Spwm=1500;// 机械手舵机PWM;

extern  u16 OUTX_Num, OUTY_Num;// 编码器捕获脉冲数,用于判断是否到位;
extern unsigned int OUTX_Flag,OUTY_Flag;// X,Y.轴电机有脉冲输出标志-1;

void MOTOR_Set(void)
{
		//	PWMA_DIS;	PWMB_DIS;	// 禁止使能 PWMA, PWMB ;X;
		//	PWMC_DIS;	PWMD_DIS;	// 禁止使能 PWMC, PWMD ;Y;
		switch(AuRun_Flag)
		{
			case 1:	// 归零位;
							if(Grid_init==0)
							{
									if((MOT_DirX!=0)||(MOT_DirY!=0))	 		 Grid0_Num++;
									else Grid0_Num=0;
									if(Grid0_Num>1)
									{
										Grid0_Num=0;
										if(OUTX_Flag==1)	OUTX_Flag=0;
										else if((OUTX_Flag==0)&&(MOT_DirX!=0)) {MOT_FlagX=1;}
										if(OUTY_Flag==1)	OUTY_Flag=0;
										else if((OUTY_Flag==0)&&(MOT_DirY!=0)) {MOT_FlagY=1;}
									}
									MOT_DirZ=2;
									MOT_Zpwm = 20000;	// 后退;
									TIM_SetCompare3(TIM4,MOT_Zpwm);					// TIM4_CRR3-Z;
									
									if(MOT_FlagX==0)
									{// 左移;
										MOT_DirX=2;				MOT_DirY=0;
									}
									else if(MOT_FlagY==0)
									{// 下移;
										MOT_DirX=0;				MOT_DirY=2;
									}
									else
									{
										Grid_init=1;	// 仅仅归行程坐标一次;
										Grid0_Num=0;
										MOT_DirX=0;				MOT_DirY=0;
										MOT_FlagX=0;			MOT_FlagY=0;
										Run_X=40;
										Run_Y=60;
									OUTX_Num	= Run_X * 330 / 4;
									OUTY_Num	= Run_Y * 330 / 8;
										if(Set_flag==2)		AuRun_Flag = 2;
										if(Set_flag==3)		AuRun_Flag = 3;
									if(Set_flag==4)		AuRun_Flag = 3;
										MOT_DirZ=0;
										MOT_Zpwm = 10000;				// 停止;
										TIM_SetCompare3(TIM4,MOT_Zpwm);					// TIM4_CRR3-Z;
									}
									
									Motor_Run();
							}
							else
							{
									MOT_DirX=0;				MOT_DirY=0;
									MOT_FlagX=0;			MOT_FlagY=0;
									if(Set_flag==2)		AuRun_Flag = 2;
									if(Set_flag==3)		AuRun_Flag = 3;
									if(Set_flag==4)		AuRun_Flag = 2;
							}
					break;
			case 2:	// 手动运行;
							if(AD_ResultX>300)// 顺时针,右移; X轴 ;
							{
								MOT_DirX=1;// 右移;
							}
							else if(AD_ResultX<30)// 逆时针,左移; X轴 ;
							{
								MOT_DirX=2;// 左移;
							}
							else MOT_DirX=0;
							
							if(AD_ResultY<30)// 顺时针,往上; Y轴;
							{
								MOT_DirY=1;// 上移;
							}
							else if(AD_ResultY>300)//逆时针,往下; Y轴;
							{
								MOT_DirY=2;// 下移;
							}
							else MOT_DirY=0;
							
							Motor_Run();
					break;
			case 3:	// 自动归位至区域起点;Start;
									if((MOT_DirX!=0)||(MOT_DirY!=0))	 		 Grid0_Num++;
									else Grid0_Num=0;
									if(Grid0_Num>3)
									{
										Grid0_Num=0;
										if(OUTX_Flag==1)	OUTX_Flag=0;
										else if((OUTX_Flag==0)&&(MOT_DirX!=0)) {MOT_FlagX=1;}
										if(OUTY_Flag==1)	OUTY_Flag=0;
										else if((OUTY_Flag==0)&&(MOT_DirY!=0)) {MOT_FlagY=1;}
											if((MOT_FlagX==1)&&(MOT_FlagY==1))
											{
												Grid0_Num=0;
												MOT_DirX=0;				MOT_DirY=0;
												MOT_FlagX=0;			MOT_FlagY=0;
															Pick_Htim=0;
												AuRun_Flag = 0;		//Alarm_Ctrl();
												
												if((Auto_end==1)&&(Apple_Pick==0))	 // 自动采摘结束;
												{
														Auto_end=2; // 自动采摘结束;
														LCD_flag = 4;// 跳转采摘结束界面;
												}
												Auto_UART=0;// 停止通讯接收;
											Motor_Run();
											break;
											}
									}
									if(MOT_FlagX==0)
									{							
							if(Run_X<Start_X)		{ MOT_DirX=1; MOT_FlagX=0; }// 右移;
							else if(Run_X>Start_X)		{ MOT_DirX=2; MOT_FlagX=0; }// 左移;
							else { 	MOT_DirX=0; MOT_FlagX=1;}
									}
									else
									{
							if(Run_Y<Start_Y)		{MOT_DirY=1; MOT_FlagY=0;}// 上移;
							else if(Run_Y>Start_Y)	{MOT_DirY=2; MOT_FlagY=0;}// 下移;
							else {  MOT_DirY=0; MOT_FlagY=1;}
									}
							
							if((MOT_FlagX==1)&&(MOT_FlagY==1))
							{
								Grid0_Num=0;
								MOT_DirX=0;				MOT_DirY=0;
								MOT_FlagX=0;			MOT_FlagY=0;
											Pick_Htim=0;
								AuRun_Flag = 0;		//Alarm_Ctrl();
								
								if((Auto_end==1)&&(Apple_Pick==0))	 // 自动采摘结束;
								{
										Auto_end=2; // 自动采摘结束;
										LCD_flag = 4;// 跳转采摘结束界面;
								}
								
								Auto_UART=0;// 停止通讯接收;
							}
							
							Motor_Run();
					break;
			case 4:	// 自动运行;
							if((PreDa_X==Start_X)&&(PreDa_Y==Start_Y))
							{
									if((MOT_DirX!=0)||(MOT_DirY!=0))	 		 Grid0_Num++;
									else Grid0_Num=0;
									if(Grid0_Num>1)
									{
										Grid0_Num=0;
										if(OUTX_Flag==1)	OUTX_Flag=0;
										else if((OUTX_Flag==0)&&(MOT_DirX!=0)) {MOT_FlagX=1;MOT_DirX=0;Pick_Htim=0;	Pick_line++;}
										if(OUTY_Flag==1)	OUTY_Flag=0;
										else if((OUTY_Flag==0)&&(MOT_DirY!=0)) {MOT_FlagY=1;}
									}
								if(Pick_line!=0)
								{
									if(MOT_FlagX==1) 
									{
										if((Run_Y<Stop_Y) && (MOT_FlagY!=1))
										{
											if(Run_Y<(Start_Y+45*(Pick_line-1)))
											{
												Pick_Htim++;
												MOT_DirY=1;// 上移;
											}
											else
											{
												MOT_DirY=0;		Pick_Htim=0;
												MOT_FlagX=0;	
											}
										}
										else
										{
											if(Pick_Htim>0)
											{
												Pick_Htim=0;		MOT_DirY=0;
												MOT_FlagX=0;		
											}
											else 
											{	MOT_FlagY=1;	}
										}
									}
									else
									{
										if((Pick_line%2)!=0) // 奇数行;
										{
											if(Run_X<Stop_X)		MOT_DirX=1;// 右移;
											else {MOT_DirX=0;MOT_FlagX=1;Pick_Htim=0;	Pick_line++;}
										}
										else if((Pick_line%2)==0)// 偶数行;
										{
											if(Run_X>Start_X)		MOT_DirX=2;// 左移;
											else {MOT_DirX=0;MOT_FlagX=1;Pick_Htim=0;	Pick_line++;}
										}
									}
								}
									if((MOT_FlagX==1)&&(MOT_FlagY==1))
									{
										Grid0_Num=0;
										MOT_DirX=0;				MOT_DirY=0;
										MOT_FlagX=0;			MOT_FlagY=0;
										AuRun_Flag=3;			Pick_line=0;
										
										Auto_end=1; // 自动采摘结束;
									}
								
								Motor_Run();
							}
							else	AuRun_Flag=5;
					break;
			case 5:	// 退回到 PreDa_X==Start_X && PreDa_Y==Start_Y 位置;采摘前位置;
							if(Run_X<PreDa_X)		{ MOT_DirX=1; MOT_FlagX=0; }// 右移;
							else if(Run_X>PreDa_X)		{ MOT_DirX=2; MOT_FlagX=0; }// 左移;
							else { 	MOT_DirX=0; MOT_FlagX=1;}
							
							if((Start_Y+45*(Pick_line-1))<Stop_Y)
							{
								if(Run_Y<(Start_Y+45*(Pick_line-1)))
								{
										MOT_DirY=1;// 上移;
										MOT_FlagY=0;
								}
								else if(Run_Y>(Start_Y+45*(Pick_line-1)))
								{
										MOT_DirY=2;// 上移;
										MOT_FlagY=0;
								}
								else
								{
										MOT_DirY=0;	
										MOT_FlagY=1;
								}
							}
							else
							{
								if(Run_Y<Stop_Y)
								{
										MOT_DirY=1;// 上移;
										MOT_FlagY=0;
								}
								else if(Run_Y<Stop_Y)
								{
										MOT_DirY=2;// 上移;
										MOT_FlagY=0;
								}
								else
								{
										MOT_DirY=0;	
										MOT_FlagY=1;
								}
							}
							
							if((MOT_FlagX==1)&&(MOT_FlagY==1))
							{
								MOT_DirX=0;				MOT_DirY=0;
								MOT_FlagX=Pre_MOT_X;
								MOT_FlagY=Pre_MOT_Y;
								PreDa_X=Start_X;	PreDa_Y=Start_Y;
								AuRun_Flag = 4;		
								if(Set_flag==3)
								{
									Auto_UART=1;// 启动通讯接收;
									AuIma_ALL=0;// 等待判定是否在屏内;//采摘前位置清零.
								}
							}
							
							Motor_Run();
					break;
			case 6:	// 追踪运动;
									if((MOT_DirX!=0)||(MOT_DirY!=0))	 		 Grid0_Num++;
									else Grid0_Num=0;
									if(Grid0_Num>4)
									{
										Grid0_Num=0;
										if(OUTX_Flag==1)	OUTX_Flag=0;
										else if((OUTX_Flag==0)&&(MOT_DirX!=0)) {MOT_FlagX=1;}
										if(OUTY_Flag==1)	OUTY_Flag=0;
										else if((OUTY_Flag==0)&&(MOT_DirY!=0)) {MOT_FlagY=1;}
										if((MOT_FlagX==1)&&(MOT_FlagY==1))
										{
											Grid0_Num=0;
											MOT_DirX=0;				MOT_DirY=0;
											MOT_FlagX=0;			MOT_FlagY=0;
											if(AuIma_ALL==1)// 全屏;追踪完成,开启采摘;
											{
												if(Set_flag==3)AuRun_Flag=4;// 回归原点继续扫描;
												if(Set_flag==4){Alarm_Ctrl();AuRun_Flag = 0;}// 退出追踪;
											}
											Motor_Run();
											break;
										}
									}
								if(Run_X<FollowDa_X)		{ MOT_DirX=1; MOT_FlagX=0; }// 右移;
								else if(Run_X>FollowDa_X)		{ MOT_DirX=2; MOT_FlagX=0; }// 左移;
								else { 	MOT_DirX=0; MOT_FlagX=1;}
								
								if(Run_Y<FollowDa_Y)		{ MOT_DirY=1; MOT_FlagY=0; }// 上移;
								else if(Run_Y>FollowDa_Y)	{ MOT_DirY=2; MOT_FlagY=0; }// 下移;
								else {  MOT_DirY=0; MOT_FlagY=1;}
								
								if((Run_X==FollowDa_X)&&(Run_Y==FollowDa_Y))
								{
									Grid0_Num=0;
									MOT_DirX=0;				MOT_DirY=0;
									MOT_FlagX=0;			MOT_FlagY=0;
									if(AuIma_ALL==1)// 全屏;追踪完成,开启采摘;
									{
										if(Set_flag==3)Apple_Pick=1;// 启动采摘;
										AuRun_Flag = 0;// 退出追踪;
										if(Set_flag==4)Alarm_Ctrl();
									}
									else	if(AuIma_ALL==2)// 半屏;
									{
										AuRun_Flag = 0;// 退出追踪;
										Auto_UART=1;// 启动通讯接收;
									}				
								}
								
							Motor_Run();
					break;
			default:break;
		}

}

void Motor_Run(void)
{
		if(MOT_DirX==1)				// 顺时针,右移; X轴 ;
		{
			OUTA_pwm=800;			OUTB_pwm=800;
		}
		else if(MOT_DirX==2)	// 逆时针,左移; X轴 ;
		{
			OUTA_pwm=200;			OUTB_pwm=200;
		}
		else MOT_DirX=0;
		
		if(MOT_DirY==1)				// 顺时针,往上; Y轴;
		{
			OUTC_pwm=200;			OUTD_pwm=800;
		}
		else if(MOT_DirY==2)	//逆时针,往下; Y轴;
		{
			OUTC_pwm=800;			OUTD_pwm=200;
		}
		else MOT_DirY=0;
		
		if((MOT_DirX!=0) && (MOT_FlagX==0))
		{
			PWMC_DIS;	PWMD_DIS;	// 禁止使能 PWMC, PWMD ;Y;
			TIM_SetCompare3(TIM1,OUTA_pwm);	// TIMx_CRR3;
			TIM_SetCompare1(TIM1,OUTB_pwm);	// TIMx_CRR1;
			PWMA_EN;	PWMB_EN;	// 使能 PWMA, PWMB ;
		}
		else if((MOT_DirY!=0) && (MOT_FlagY==0))
		{
			PWMA_DIS;	PWMB_DIS;	// 禁止使能 PWMA, PWMB ;X;
			TIM_SetCompare1(TIM1,OUTC_pwm);	// TIM1_CRR1;
			TIM_SetCompare2(TIM1,OUTD_pwm);	// TIM1_CRR2;
			PWMC_EN;	PWMD_EN;	// 使能 PWMC, PWMD ;
		}
		else
		{
			PWMA_DIS;	PWMB_DIS;	// 禁止使能 PWMA, PWMB ;X;
			PWMC_DIS;	PWMD_DIS;	// 禁止使能 PWMC, PWMD ;Y;
		}
}


