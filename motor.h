#ifndef __motor_H
#define __motor_H

#include "stm32f10x.h"

#define Pick_NumMax 	20				//  最大能存储采摘水果坐标个数;
#define SetX_max 			460				//  区域设定: X轴坐标最大值;
#define SetY_max 			360				//  区域设定: Y轴坐标最大值;
#define SrX 			100						//  区域设定 预设值: Start_X 
#define SpX 			300						//  区域设定 预设值: Stop_X 
#define SrY				100						//  区域设定 预设值: Start_Y 
#define SpY				300						//  区域设定 预设值: Stop_Y 
#define MOT_Spwm_M				1500	// 	机械手舵机正常状态;m
//#define MOT_Spwm_MAX 			1650	// 	机械手舵机伸缩最大间距;max
//#define MOT_Spwm_MIN 			1420	// 	机械手舵机伸缩最小间距;min
//#define Pick_time_M				70		// 	Z轴出去或回来最大延时;

/* extern Private variables ---------------------------------------------------------*/
// 按键部分;
extern unsigned char press_flag1,press_flag2,press_flag3,press_flag4;// 独立按键1.2.3.4;
extern unsigned char	key_Pressed,key_val,key_flag;// 矩阵按键键值;
// 界面及显示部分;
extern unsigned char LCD_flag;// 界面号: 0=>LCD_Start; 1=>LCD_Main; 2=>LCD_Set; 3=>LCD_State; 4=>坐标点图;
extern unsigned char LCD_Ying;// LCD闪烁标志;
extern unsigned char Set_flag;// 功能: 0=>主界面; 1=>区域设定; 2=>手动采摘; 3=>自动采摘;
extern unsigned int Start_X,Start_Y,Stop_X,Stop_Y,Run_X,Run_Y;// 起点坐标, 终点坐标; 机械手坐标;
extern unsigned int Run_Num,Run_V;// 数量, 压力;
extern unsigned int Set_Data[12];// 区域设定界面参数设定值;
extern unsigned char Set_Wei;// 区域设定界面参数位;
// 上电归零位部分;
extern unsigned int  Auto_end;
		//Auto_end 自动运行标志:0-正常自动运行;1-自动运行结束动作;2-自动运行已结束(等待退出界面);
extern unsigned int  Grid_init, Grid0_Num;
		//Grid_init 归零位标志:1-已完成上电归零位操作,无需再做;
		//Grid0_Num 归零位操作:识别脉冲有无延时;
// ADC 部分;
extern unsigned char AN1_flag;//AN1_flag 摇杆按键;
extern unsigned long int AD_ResultX, AD_ResultY, AD_ResultV;
		//AD_ResultX 摇杆ADC3;//AD_ResultY 摇杆ADC2;//AD_ResultV 压力;
extern unsigned long int AD_RunV_over;//AD_RunV_over 超压力标志;
// 采摘控制部分;
extern unsigned int  AuRun_Flag, Auto_UART, AuIma_ALL, Apple_Pick;
		//AuRun_Flag 电机动作:1-归零位(上电仅一次);2-手动操作;3-归区域零点;4-自动运行;5-归追踪前位置;6-追踪目标;
		//Auto_UART 1-开启通信接收;0-禁止通信接收;
		//AuIma_ALL 启动自动扫描及回到发现点原点时归0,全屏为 1 ,半屏为 2 ;
		//Apple_Pick 水果采摘动作分级;0-无动作;1-开始动作;2,3,4,....直至动作结束归0;
extern unsigned int  Pick_time1,Pick_time2;// 手动采摘机械手前进,后退计时;
extern unsigned int  Pick_line,Pick_Htim;//自动运行时的行数,列上升计时;
extern unsigned int  PreDa_X,PreDa_Y, FollowDa_X,FollowDa_Y;
		//PreDa_X,PreDa_Y 采摘追踪前坐标;
		//FollowDa_X,FollowDa_Y 被追踪目标坐标;
extern unsigned int  Pick_DaX[Pick_NumMax],Pick_DaY[Pick_NumMax];// 采摘水果动作坐标;
// 采摘参数可设置变量;
extern unsigned int  MOT_Spwm_MAX, MOT_Spwm_MIN; 	// 	机械手舵机伸缩最大间距;max; 最小间距;min
extern unsigned int  Pick_time_M;					// 	Z轴出去或回来最大延时;
extern unsigned int  shift_X,shift_Y;			// 自动采摘时 X,Y轴摄像头偏移量;
// 电机部分;
extern unsigned int MOT_DirX,MOT_DirY,MOT_DirZ;// X,Y,Z.电机方向: 0,停止; 1,右移,上移,前进; 2,左移,下移,后退;,MOT_DirV
extern unsigned int MOT_FlagX, MOT_FlagY;// X轴电机动作标志;// Y轴电机动作标志;
extern unsigned int Pre_MOT_X,Pre_MOT_Y;// 自动运行:采摘追踪前X Y轴电机动作标志;
// PWM部分;
extern unsigned int OUTA_pwm, OUTB_pwm, OUTC_pwm, OUTD_pwm;// A,B,C,D.PWM;
extern unsigned int MOT_Zpwm, MOT_Spwm;// Z轴电机ZPWM;// 机械手舵机SPWM;
//- Private variables end -----------------------------------------------------------//

void MOTOR_Set(void);
void Motor_Run(void);
//void Set0_Home(void);
//void Set1_Area(void);
//void Area_Init(void);
//void Set2_Hand(void);
//void Set3_Auto(void);

#endif

