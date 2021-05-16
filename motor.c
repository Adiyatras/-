#include "stm32f10x.h"
#include  "stm32f10x_conf.h" 
#include  "bsp.h"
#include  "adc.h"
#include  "PWM.h"
#include  "motor.h"
#include  "LCD12864.h"
#include  "keypad.h"
#include  "operate.h"

// ��������;
unsigned char press_flag1=0;// ��������1;
unsigned char press_flag2=0;// ��������2;
unsigned char press_flag3=0;// ��������3;
unsigned char press_flag4=0;// ��������4;
unsigned char	key_Pressed,key_flag;
unsigned char	key_val=0;// ���󰴼���ֵ;
// ������ʾ����;
unsigned char LCD_flag=0;// �����: 0=>LCD_Start; 1=>LCD_Main; 2=>LCD_Set; 3=>LCD_State; 4=>�����ͼ;
unsigned char LCD_Ying=0;// LCD��˸��־;
unsigned char Set_flag=0;// ����: 0=>������; 1=>�����趨; 2=>�ֶ���ժ; 3=>�Զ���ժ;
unsigned int Start_X,Start_Y,Stop_X,Stop_Y,Run_X,Run_Y;// �������, �յ�����; ��е������;
unsigned int Run_Num,Run_V;// ����, ѹ��;
unsigned int Set_Data[12];// �����趨��������趨ֵ;
unsigned char Set_Wei=0;// �����趨�������λ;
// �ϵ����λ����;
unsigned int  Auto_end=0;// �Զ����б�־:0-�����Զ�����;1-�Զ����н�������;2-�Զ������ѽ���(�ȴ��˳�����);
unsigned int  Grid_init=0;// ����λ��־:1-������ϵ����λ����,��������;
unsigned int  Grid0_Num=0;// ����λ����:ʶ������������ʱ;
// ADC ����;
unsigned char AN1_flag=0;// ҡ�˰���;
unsigned long int AD_ResultX=150;// ҡ��ADC3;
unsigned long int AD_ResultY=150;// ҡ��ADC2;
unsigned long int AD_ResultV=150;// ѹ��;
unsigned long int AD_RunV_over=0;// ��ѹ����־;
// ��ժ���Ʋ���;
unsigned int  AuRun_Flag=0;// �������:1-����λ(�ϵ��һ��);2-�ֶ�����;3-���������;4-�Զ�����;5-��׷��ǰλ��;6-׷��Ŀ��;
unsigned int  Auto_UART=0;// 1-����ͨ�Ž���;0-��ֹͨ�Ž���;
unsigned int  AuIma_ALL=0;// �����Զ�ɨ�輰�ص����ֵ�ԭ��ʱ��0,ȫ��Ϊ 1 ,����Ϊ 2 ;
unsigned int  Apple_Pick=0;// ˮ����ժ�����ּ�;0-�޶���;1-��ʼ����;2,3,4,....ֱ������������0;
unsigned int  Pick_time1,Pick_time2;// �ֶ���ժ��е��ǰ��,���˼�ʱ;
unsigned int  Pick_line,Pick_Htim;//�Զ�����ʱ������,��������ʱ;
unsigned int  PreDa_X,PreDa_Y;// ��ժ׷��ǰ����;
unsigned int  FollowDa_X,FollowDa_Y;// ��׷��Ŀ������;
unsigned int  Pick_DaX[Pick_NumMax],Pick_DaY[Pick_NumMax];// ��ժˮ����������;
// ��ժ���������ñ���;
unsigned int  MOT_Spwm_MAX=1600; 				// 	��е�ֶ�����������;max;1500~1750�ɵ�;
unsigned int  MOT_Spwm_MIN=1400; 				// 	��е�ֶ��������С���;min;1250~1500�ɵ�;
unsigned int  Pick_time_M=14;						// 	Z���ȥ����������ʱ;
unsigned int  shift_X=40;								// �Զ���ժʱ X,Y������ͷƫ����;X:0~80�ɵ�;Y:0~100�ɵ�;
unsigned int  shift_Y=50;								// �Զ���ժʱ X,Y������ͷƫ����;X:0~80�ɵ�;Y:0~100�ɵ�;
// �������;
unsigned int MOT_DirX,MOT_DirY,MOT_DirZ;// X,Y,Z.�������: 0,ֹͣ; 1,����,����,ǰ��; 2,����,����,����;,MOT_DirV
unsigned int MOT_FlagX=0;// X����������־;
unsigned int MOT_FlagY=0;// Y����������־;
unsigned int Pre_MOT_X,Pre_MOT_Y;// �Զ�����:��ժ׷��ǰX Y����������־;
// PWM����;
unsigned int OUTA_pwm=500;// A,B,C,D.PWM;
unsigned int OUTB_pwm=500;
unsigned int OUTC_pwm=500;
unsigned int OUTD_pwm=500;
unsigned int MOT_Zpwm=10000;// Z����PWM;
unsigned int MOT_Spwm=1500;// ��е�ֶ��PWM;

extern  u16 OUTX_Num, OUTY_Num;// ����������������,�����ж��Ƿ�λ;
extern unsigned int OUTX_Flag,OUTY_Flag;// X,Y.���������������־-1;

void MOTOR_Set(void)
{
		//	PWMA_DIS;	PWMB_DIS;	// ��ֹʹ�� PWMA, PWMB ;X;
		//	PWMC_DIS;	PWMD_DIS;	// ��ֹʹ�� PWMC, PWMD ;Y;
		switch(AuRun_Flag)
		{
			case 1:	// ����λ;
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
									MOT_Zpwm = 20000;	// ����;
									TIM_SetCompare3(TIM4,MOT_Zpwm);					// TIM4_CRR3-Z;
									
									if(MOT_FlagX==0)
									{// ����;
										MOT_DirX=2;				MOT_DirY=0;
									}
									else if(MOT_FlagY==0)
									{// ����;
										MOT_DirX=0;				MOT_DirY=2;
									}
									else
									{
										Grid_init=1;	// �������г�����һ��;
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
										MOT_Zpwm = 10000;				// ֹͣ;
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
			case 2:	// �ֶ�����;
							if(AD_ResultX>300)// ˳ʱ��,����; X�� ;
							{
								MOT_DirX=1;// ����;
							}
							else if(AD_ResultX<30)// ��ʱ��,����; X�� ;
							{
								MOT_DirX=2;// ����;
							}
							else MOT_DirX=0;
							
							if(AD_ResultY<30)// ˳ʱ��,����; Y��;
							{
								MOT_DirY=1;// ����;
							}
							else if(AD_ResultY>300)//��ʱ��,����; Y��;
							{
								MOT_DirY=2;// ����;
							}
							else MOT_DirY=0;
							
							Motor_Run();
					break;
			case 3:	// �Զ���λ���������;Start;
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
												
												if((Auto_end==1)&&(Apple_Pick==0))	 // �Զ���ժ����;
												{
														Auto_end=2; // �Զ���ժ����;
														LCD_flag = 4;// ��ת��ժ��������;
												}
												Auto_UART=0;// ֹͣͨѶ����;
											Motor_Run();
											break;
											}
									}
									if(MOT_FlagX==0)
									{							
							if(Run_X<Start_X)		{ MOT_DirX=1; MOT_FlagX=0; }// ����;
							else if(Run_X>Start_X)		{ MOT_DirX=2; MOT_FlagX=0; }// ����;
							else { 	MOT_DirX=0; MOT_FlagX=1;}
									}
									else
									{
							if(Run_Y<Start_Y)		{MOT_DirY=1; MOT_FlagY=0;}// ����;
							else if(Run_Y>Start_Y)	{MOT_DirY=2; MOT_FlagY=0;}// ����;
							else {  MOT_DirY=0; MOT_FlagY=1;}
									}
							
							if((MOT_FlagX==1)&&(MOT_FlagY==1))
							{
								Grid0_Num=0;
								MOT_DirX=0;				MOT_DirY=0;
								MOT_FlagX=0;			MOT_FlagY=0;
											Pick_Htim=0;
								AuRun_Flag = 0;		//Alarm_Ctrl();
								
								if((Auto_end==1)&&(Apple_Pick==0))	 // �Զ���ժ����;
								{
										Auto_end=2; // �Զ���ժ����;
										LCD_flag = 4;// ��ת��ժ��������;
								}
								
								Auto_UART=0;// ֹͣͨѶ����;
							}
							
							Motor_Run();
					break;
			case 4:	// �Զ�����;
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
												MOT_DirY=1;// ����;
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
										if((Pick_line%2)!=0) // ������;
										{
											if(Run_X<Stop_X)		MOT_DirX=1;// ����;
											else {MOT_DirX=0;MOT_FlagX=1;Pick_Htim=0;	Pick_line++;}
										}
										else if((Pick_line%2)==0)// ż����;
										{
											if(Run_X>Start_X)		MOT_DirX=2;// ����;
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
										
										Auto_end=1; // �Զ���ժ����;
									}
								
								Motor_Run();
							}
							else	AuRun_Flag=5;
					break;
			case 5:	// �˻ص� PreDa_X==Start_X && PreDa_Y==Start_Y λ��;��ժǰλ��;
							if(Run_X<PreDa_X)		{ MOT_DirX=1; MOT_FlagX=0; }// ����;
							else if(Run_X>PreDa_X)		{ MOT_DirX=2; MOT_FlagX=0; }// ����;
							else { 	MOT_DirX=0; MOT_FlagX=1;}
							
							if((Start_Y+45*(Pick_line-1))<Stop_Y)
							{
								if(Run_Y<(Start_Y+45*(Pick_line-1)))
								{
										MOT_DirY=1;// ����;
										MOT_FlagY=0;
								}
								else if(Run_Y>(Start_Y+45*(Pick_line-1)))
								{
										MOT_DirY=2;// ����;
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
										MOT_DirY=1;// ����;
										MOT_FlagY=0;
								}
								else if(Run_Y<Stop_Y)
								{
										MOT_DirY=2;// ����;
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
									Auto_UART=1;// ����ͨѶ����;
									AuIma_ALL=0;// �ȴ��ж��Ƿ�������;//��ժǰλ������.
								}
							}
							
							Motor_Run();
					break;
			case 6:	// ׷���˶�;
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
											if(AuIma_ALL==1)// ȫ��;׷�����,������ժ;
											{
												if(Set_flag==3)AuRun_Flag=4;// �ع�ԭ�����ɨ��;
												if(Set_flag==4){Alarm_Ctrl();AuRun_Flag = 0;}// �˳�׷��;
											}
											Motor_Run();
											break;
										}
									}
								if(Run_X<FollowDa_X)		{ MOT_DirX=1; MOT_FlagX=0; }// ����;
								else if(Run_X>FollowDa_X)		{ MOT_DirX=2; MOT_FlagX=0; }// ����;
								else { 	MOT_DirX=0; MOT_FlagX=1;}
								
								if(Run_Y<FollowDa_Y)		{ MOT_DirY=1; MOT_FlagY=0; }// ����;
								else if(Run_Y>FollowDa_Y)	{ MOT_DirY=2; MOT_FlagY=0; }// ����;
								else {  MOT_DirY=0; MOT_FlagY=1;}
								
								if((Run_X==FollowDa_X)&&(Run_Y==FollowDa_Y))
								{
									Grid0_Num=0;
									MOT_DirX=0;				MOT_DirY=0;
									MOT_FlagX=0;			MOT_FlagY=0;
									if(AuIma_ALL==1)// ȫ��;׷�����,������ժ;
									{
										if(Set_flag==3)Apple_Pick=1;// ������ժ;
										AuRun_Flag = 0;// �˳�׷��;
										if(Set_flag==4)Alarm_Ctrl();
									}
									else	if(AuIma_ALL==2)// ����;
									{
										AuRun_Flag = 0;// �˳�׷��;
										Auto_UART=1;// ����ͨѶ����;
									}				
								}
								
							Motor_Run();
					break;
			default:break;
		}

}

void Motor_Run(void)
{
		if(MOT_DirX==1)				// ˳ʱ��,����; X�� ;
		{
			OUTA_pwm=800;			OUTB_pwm=800;
		}
		else if(MOT_DirX==2)	// ��ʱ��,����; X�� ;
		{
			OUTA_pwm=200;			OUTB_pwm=200;
		}
		else MOT_DirX=0;
		
		if(MOT_DirY==1)				// ˳ʱ��,����; Y��;
		{
			OUTC_pwm=200;			OUTD_pwm=800;
		}
		else if(MOT_DirY==2)	//��ʱ��,����; Y��;
		{
			OUTC_pwm=800;			OUTD_pwm=200;
		}
		else MOT_DirY=0;
		
		if((MOT_DirX!=0) && (MOT_FlagX==0))
		{
			PWMC_DIS;	PWMD_DIS;	// ��ֹʹ�� PWMC, PWMD ;Y;
			TIM_SetCompare3(TIM1,OUTA_pwm);	// TIMx_CRR3;
			TIM_SetCompare1(TIM1,OUTB_pwm);	// TIMx_CRR1;
			PWMA_EN;	PWMB_EN;	// ʹ�� PWMA, PWMB ;
		}
		else if((MOT_DirY!=0) && (MOT_FlagY==0))
		{
			PWMA_DIS;	PWMB_DIS;	// ��ֹʹ�� PWMA, PWMB ;X;
			TIM_SetCompare1(TIM1,OUTC_pwm);	// TIM1_CRR1;
			TIM_SetCompare2(TIM1,OUTD_pwm);	// TIM1_CRR2;
			PWMC_EN;	PWMD_EN;	// ʹ�� PWMC, PWMD ;
		}
		else
		{
			PWMA_DIS;	PWMB_DIS;	// ��ֹʹ�� PWMA, PWMB ;X;
			PWMC_DIS;	PWMD_DIS;	// ��ֹʹ�� PWMC, PWMD ;Y;
		}
}


