#ifndef __motor_H
#define __motor_H

#include "stm32f10x.h"

#define Pick_NumMax 	20				//  ����ܴ洢��ժˮ���������;
#define SetX_max 			460				//  �����趨: X���������ֵ;
#define SetY_max 			360				//  �����趨: Y���������ֵ;
#define SrX 			100						//  �����趨 Ԥ��ֵ: Start_X 
#define SpX 			300						//  �����趨 Ԥ��ֵ: Stop_X 
#define SrY				100						//  �����趨 Ԥ��ֵ: Start_Y 
#define SpY				300						//  �����趨 Ԥ��ֵ: Stop_Y 
#define MOT_Spwm_M				1500	// 	��е�ֶ������״̬;m
//#define MOT_Spwm_MAX 			1650	// 	��е�ֶ�����������;max
//#define MOT_Spwm_MIN 			1420	// 	��е�ֶ��������С���;min
//#define Pick_time_M				70		// 	Z���ȥ����������ʱ;

/* extern Private variables ---------------------------------------------------------*/
// ��������;
extern unsigned char press_flag1,press_flag2,press_flag3,press_flag4;// ��������1.2.3.4;
extern unsigned char	key_Pressed,key_val,key_flag;// ���󰴼���ֵ;
// ���漰��ʾ����;
extern unsigned char LCD_flag;// �����: 0=>LCD_Start; 1=>LCD_Main; 2=>LCD_Set; 3=>LCD_State; 4=>�����ͼ;
extern unsigned char LCD_Ying;// LCD��˸��־;
extern unsigned char Set_flag;// ����: 0=>������; 1=>�����趨; 2=>�ֶ���ժ; 3=>�Զ���ժ;
extern unsigned int Start_X,Start_Y,Stop_X,Stop_Y,Run_X,Run_Y;// �������, �յ�����; ��е������;
extern unsigned int Run_Num,Run_V;// ����, ѹ��;
extern unsigned int Set_Data[12];// �����趨��������趨ֵ;
extern unsigned char Set_Wei;// �����趨�������λ;
// �ϵ����λ����;
extern unsigned int  Auto_end;
		//Auto_end �Զ����б�־:0-�����Զ�����;1-�Զ����н�������;2-�Զ������ѽ���(�ȴ��˳�����);
extern unsigned int  Grid_init, Grid0_Num;
		//Grid_init ����λ��־:1-������ϵ����λ����,��������;
		//Grid0_Num ����λ����:ʶ������������ʱ;
// ADC ����;
extern unsigned char AN1_flag;//AN1_flag ҡ�˰���;
extern unsigned long int AD_ResultX, AD_ResultY, AD_ResultV;
		//AD_ResultX ҡ��ADC3;//AD_ResultY ҡ��ADC2;//AD_ResultV ѹ��;
extern unsigned long int AD_RunV_over;//AD_RunV_over ��ѹ����־;
// ��ժ���Ʋ���;
extern unsigned int  AuRun_Flag, Auto_UART, AuIma_ALL, Apple_Pick;
		//AuRun_Flag �������:1-����λ(�ϵ��һ��);2-�ֶ�����;3-���������;4-�Զ�����;5-��׷��ǰλ��;6-׷��Ŀ��;
		//Auto_UART 1-����ͨ�Ž���;0-��ֹͨ�Ž���;
		//AuIma_ALL �����Զ�ɨ�輰�ص����ֵ�ԭ��ʱ��0,ȫ��Ϊ 1 ,����Ϊ 2 ;
		//Apple_Pick ˮ����ժ�����ּ�;0-�޶���;1-��ʼ����;2,3,4,....ֱ������������0;
extern unsigned int  Pick_time1,Pick_time2;// �ֶ���ժ��е��ǰ��,���˼�ʱ;
extern unsigned int  Pick_line,Pick_Htim;//�Զ�����ʱ������,��������ʱ;
extern unsigned int  PreDa_X,PreDa_Y, FollowDa_X,FollowDa_Y;
		//PreDa_X,PreDa_Y ��ժ׷��ǰ����;
		//FollowDa_X,FollowDa_Y ��׷��Ŀ������;
extern unsigned int  Pick_DaX[Pick_NumMax],Pick_DaY[Pick_NumMax];// ��ժˮ����������;
// ��ժ���������ñ���;
extern unsigned int  MOT_Spwm_MAX, MOT_Spwm_MIN; 	// 	��е�ֶ�����������;max; ��С���;min
extern unsigned int  Pick_time_M;					// 	Z���ȥ����������ʱ;
extern unsigned int  shift_X,shift_Y;			// �Զ���ժʱ X,Y������ͷƫ����;
// �������;
extern unsigned int MOT_DirX,MOT_DirY,MOT_DirZ;// X,Y,Z.�������: 0,ֹͣ; 1,����,����,ǰ��; 2,����,����,����;,MOT_DirV
extern unsigned int MOT_FlagX, MOT_FlagY;// X����������־;// Y����������־;
extern unsigned int Pre_MOT_X,Pre_MOT_Y;// �Զ�����:��ժ׷��ǰX Y����������־;
// PWM����;
extern unsigned int OUTA_pwm, OUTB_pwm, OUTC_pwm, OUTD_pwm;// A,B,C,D.PWM;
extern unsigned int MOT_Zpwm, MOT_Spwm;// Z����ZPWM;// ��е�ֶ��SPWM;
//- Private variables end -----------------------------------------------------------//

void MOTOR_Set(void);
void Motor_Run(void);
//void Set0_Home(void);
//void Set1_Area(void);
//void Area_Init(void);
//void Set2_Hand(void);
//void Set3_Auto(void);

#endif

