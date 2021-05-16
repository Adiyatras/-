#include "stm32f10x.h"
#include  "stm32f10x_conf.h" 
#include  "bsp.h"
#include  "PWM.h"
#include  "motor.h"
#include  "keypad.h"
#include  "LCD12864.h"
#include  "operate.h"

void Set0_Home(void)
{
		u16 ix;
		switch(key_val)
		{
			case 10:// 1.区域设定 ;
							key_val=0;
							Set_flag=1;		LCD_flag=2;
							TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);	//使能时钟TIM2定时中断源中断
							TIM_Cmd(TIM2, ENABLE);											//使能定时器2
				break;
			case 11:// 2.手动采摘 ;
							key_val=0;
							Set_flag=2;		LCD_flag=3;
							AuRun_Flag=1;//归零位;
							Run_Num=0;// 果实计数清零;
							for(ix = 0; ix < Pick_NumMax; ix++)
							{
								Pick_DaX[ix]=0;
								Pick_DaY[ix]=0;
							}
				break;
			case 12:// 3.自动采摘 ;
							key_val=0;
							Set_flag=3;		LCD_flag=3;
							AuRun_Flag=1;//归零位;
							Auto_end=0;
							Run_Num=0;// 果实计数清零;
							for(ix = 0; ix < Pick_NumMax; ix++)
							{
								Pick_DaX[ix]=0;
								Pick_DaY[ix]=0;
							}
				break;
			case 6:// 4.校准 ;
							key_val=0;
							Set_flag=4;		LCD_flag=5;
							AuRun_Flag=1;//归零位;
							TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);	//使能时钟TIM2定时中断源中断
							TIM_Cmd(TIM2, ENABLE);											//使能定时器2
				break;
			default:break;
		}
			PWMA_DIS;	PWMB_DIS;	// 禁止使能 PWMA, PWMB ;X;
			PWMC_DIS;	PWMD_DIS;	// 禁止使能 PWMC, PWMD ;Y;
		
}

void Set2_Hand(void)
{
		if(press_flag1==1)// 退回主界面 ;
		{
					key_val=0;
			Set_flag=0;LCD_flag=1;
			AuRun_Flag=0;	// 禁止电机操作;
		}

		if((AN1_flag==1)&&(AuRun_Flag!=1))
		{
			switch(Apple_Pick)
			{
				case 0:	// 确认采摘;// 正常00;
								Pick_time1=0;
								Pick_time2=0;
								MOT_DirZ=0;
								MOT_Spwm=MOT_Spwm_M;
								Apple_Pick=1;
								AuRun_Flag=0;	// 禁止电机操作;
								break;
				case 1:	// 正常01;
								Pick_time1++;			MOT_DirZ=1;// Z轴电机前进;
								if(MOT_Spwm<MOT_Spwm_MAX)	MOT_Spwm=MOT_Spwm+10;
								if(Pick_time1>Pick_time_M*5)		{	Pick_time1=0;	Apple_Pick=2;	}
								break;
				case 2:	// 正常02;
								Pick_time1++;			MOT_DirZ=0;
								if((MOT_Spwm>MOT_Spwm_MIN))	MOT_Spwm=MOT_Spwm-10;//&&(AD_RunV_over!=1)
								if(Pick_time1>30)	{	Pick_time1=0; Apple_Pick=3;}
								break;
				case 3:	MOT_DirZ=0;	// 正常03;
								break;
				case 4:		// 正常04;
								Pick_time2++;			MOT_DirZ=2;// 正常后退;
								Pick_DaX[Run_Num]=Run_X;
								Pick_DaY[Run_Num]=Run_Y;
								if(Pick_time2>(Pick_time_M*5))	{AuRun_Flag=3;Pick_time2=0;Apple_Pick=5;}
								break;
				case 5:
								if(AuRun_Flag==0)	{Apple_Pick=6;}	// 电机归零位;
								break;
				case 6:		// 正常05;
								Pick_time2++;			MOT_DirZ=0;
								if(MOT_Spwm<MOT_Spwm_MAX)	MOT_Spwm=MOT_Spwm+10;
								if(Pick_time2>30){Pick_time2=0;Apple_Pick=7;Run_Num++;Alarm_Ctrl();}// 果实计数;
								break;
				case 7:		// 结束采摘;归0;
								Pick_time2++;
								if(MOT_Spwm>MOT_Spwm_M)	MOT_Spwm=MOT_Spwm-10;
								if(Pick_time2>30)
								{
									Pick_time2=0;
									MOT_DirZ=0;
									MOT_Spwm=MOT_Spwm_M;
									Apple_Pick=0;
									AuRun_Flag=2;
								}
								break;
				case 8:	Pick_time1=(Pick_time_M*5)-Pick_time2;			MOT_DirZ=0;
								Apple_Pick=1;
								break;
				case 9:	break;	// 归正常06;不打扰;
				default:	break;
			}
			if(MOT_DirZ==0) MOT_Zpwm = 10000;
			else if(MOT_DirZ==1) MOT_Zpwm = 0;
			else if(MOT_DirZ==2) MOT_Zpwm = 20000;
			TIM_SetCompare3(TIM4,MOT_Zpwm);							// TIM4_CRR3-Z;
			TIM_SetCompare4(TIM4,MOT_Spwm);							// TIM4_CRR4;
			
		}
		else if((AN1_flag==0)&&(Apple_Pick!=0))
		{
			switch(Apple_Pick) 
			{
				case 1:		// 01异常;
								Pick_time2=(Pick_time_M*5)-Pick_time1;		MOT_DirZ=0;
								Apple_Pick=8;
								break;
				case 2:		// 02异常;
								Pick_time2=Pick_time1;		MOT_DirZ=0;
								Apple_Pick=9;
								break;
				case 3:		// 正常03;
								Pick_time2=0;		MOT_DirZ=0;
								Apple_Pick=4;
								break;
				case 4:		// 正常04;
								Pick_time2++;			MOT_DirZ=2;// 正常后退;
								Pick_DaX[Run_Num]=Run_X;
								Pick_DaY[Run_Num]=Run_Y;
								if(Pick_time2>(Pick_time_M*5))	{AuRun_Flag=3;Pick_time2=0;Apple_Pick=5;}
								break;
				case 5:
								if(AuRun_Flag==0)	{Apple_Pick=6;}	// 电机归零位;
								break;
				case 6:		// 正常05;
								Pick_time2++;			MOT_DirZ=0;
								if(MOT_Spwm<MOT_Spwm_MAX)	MOT_Spwm=MOT_Spwm+10;
								if(Pick_time2>30){Pick_time2=0;Apple_Pick=7;Run_Num++;Alarm_Ctrl();}// 果实计数;
								break;
				case 7:		// 结束采摘;归0;
								Pick_time2++;
								if(MOT_Spwm>MOT_Spwm_M)	MOT_Spwm=MOT_Spwm-10;
								if(Pick_time2>30)
								{
									Pick_time2=0;
									MOT_DirZ=0;
									MOT_Spwm=MOT_Spwm_M;
									Apple_Pick=0;
									AuRun_Flag=2;
								}
								break;
				case 8:		// 01异常;
								Pick_time2++;			MOT_DirZ=2;
								if(Pick_time2>(Pick_time_M*5))	{Pick_time2=0;Apple_Pick=7;}
								break;
				case 9:		// 02异常;
								Pick_time2++;			MOT_DirZ=0;	
								if((MOT_Spwm>MOT_Spwm_MIN))		MOT_Spwm=MOT_Spwm-10;//&&(AD_RunV_over!=1)
								if(Pick_time2>30)		{Pick_time2=0;Apple_Pick=6;}
								break;
				default:	break;
			}
			if(MOT_DirZ==0) MOT_Zpwm = 10000;
			else if(MOT_DirZ==1) MOT_Zpwm = 0;
			else if(MOT_DirZ==2) MOT_Zpwm = 20000;
			TIM_SetCompare3(TIM4,MOT_Zpwm);					// TIM4_CRR3-Z;
			TIM_SetCompare4(TIM4,MOT_Spwm);					// TIM4_CRR4;
		}
		MOTOR_Set();				// 电机运行;
}

void Set3_Auto(void)
{
			if(press_flag1==1)	// 退回主界面 ;
			{
					key_val=0;
				Set_flag=0;LCD_flag=1;
				LCD_Clear_Screen();	   //清屏函数 防止乱码
				AuRun_Flag=0;			// 电机无操作;
				Auto_end=0;
					Auto_UART=0;			// 停止通讯接收;
			}
			
			if(Auto_end==0) // 自动采摘
			{
				if((key_val==16)&&(AuRun_Flag==0)&&(Auto_UART==0))// 按 E 开始电机自动扫描
				{
					key_val=0;
					PreDa_X=Start_X;
					PreDa_Y=Start_Y;
					Pick_line=1;
					AuRun_Flag=4;			// 电机自动扫描;
					Auto_UART=1;			// 启动通讯接收;
					AuIma_ALL=0;// 等待判定是否在屏内;
				}
				if(key_val==14)// 按 D 结束电机自动扫描  &&(AuRun_Flag==4))
				{
					key_val=0;
					//AuRun_Flag=3;			// 归区域零点;
					Auto_UART=0;			// 停止通讯接收;
					Auto_end=1; 			// 自动采摘结束;
					//LCD_flag = 4;			// 跳转采摘结束界面;
				}


				if(Apple_Pick!=0)															// 采摘中;
				{
					switch(Apple_Pick)
					{
						case 1:	// 确认采摘;// 正常00;
										Pick_time1=0;
										MOT_DirZ=0;
										MOT_Spwm=MOT_Spwm_M;
										Apple_Pick=2;
										AuRun_Flag=0;	// 禁止电机操作;
										break;
						case 2:	// 正常02;
										Pick_time1++;			MOT_DirZ=1;// Z轴电机前进;
										if(MOT_Spwm<MOT_Spwm_MAX)	MOT_Spwm=MOT_Spwm+10;
										if(Pick_time1>(Pick_time_M*5))		{	Pick_time1=0;	Apple_Pick=3;	}
										break;
						case 3:	// 正常03;
										Pick_time1++;			MOT_DirZ=0;
										if((MOT_Spwm>MOT_Spwm_MIN))	MOT_Spwm=MOT_Spwm-10;//&&(AD_RunV_over!=1)
										if(Pick_time1>30)	{	Pick_time1=0; Apple_Pick=4;}
										break;
						case 4:		// 正常04;
										Pick_time1++;			MOT_DirZ=2;// 正常后退;
										Pick_DaX[Run_Num]=Run_X;
										Pick_DaY[Run_Num]=Run_Y;
										if(Pick_time1>(Pick_time_M*5))	{AuRun_Flag=3;Pick_time1=0;Apple_Pick=5;}
										break;
						case 5:
										if(AuRun_Flag==0)	{Apple_Pick=6;}	// 电机归零位;
										break;
						case 6:		// 正常05;
										Pick_time1++;			MOT_DirZ=0;
										if(MOT_Spwm<MOT_Spwm_MAX)	MOT_Spwm=MOT_Spwm+10;
										if(Pick_time1>30){Pick_time1=0;Apple_Pick=7;Run_Num++;Alarm_Ctrl();}// 果实计数;
										break;
						case 7:		// 结束采摘;归0;
										Pick_time1++;
										if(MOT_Spwm>MOT_Spwm_M)	MOT_Spwm=MOT_Spwm-10;
										if(Pick_time1>30)
										{
											Pick_time1=0;
											MOT_DirZ=0;
											MOT_Spwm=MOT_Spwm_M;
											Apple_Pick=0;
											AuRun_Flag=4;
										}
										break;
						default:break;
					}
					if(MOT_DirZ==0) MOT_Zpwm = 10000;				// 停止;
					else if(MOT_DirZ==1) MOT_Zpwm = 0;			// 前进;
					else if(MOT_DirZ==2) MOT_Zpwm = 20000;	// 后退;
					TIM_SetCompare3(TIM4,MOT_Zpwm);					// TIM4_CRR3-Z;
					TIM_SetCompare4(TIM4,MOT_Spwm);					// TIM4_CRR4;
				}

				MOTOR_Set();				// 电机运行;
			}
			else if(Auto_end==1)// 自动采摘结束;
			{
				if(Apple_Pick!=0)															// 采摘中;
				{
					switch(Apple_Pick)
					{
						case 1:	Apple_Pick=7;
										break;
						case 2:	
										Pick_time1++;			MOT_DirZ=1;// Z轴电机前进;
										if(MOT_Spwm<MOT_Spwm_MAX)	MOT_Spwm=MOT_Spwm+10;
										if(Pick_time1>(Pick_time_M*5))		{	Pick_time1=0;	Apple_Pick=3;	}
										break;
						case 3:	// 正常03;
										Pick_time1++;			MOT_DirZ=0;
										if((MOT_Spwm>MOT_Spwm_MIN))	MOT_Spwm=MOT_Spwm-10;//&&(AD_RunV_over!=1)
										if(Pick_time1>30)	{	Pick_time1=0; Apple_Pick=4;}
										break;
						case 4:		// 正常04;
										Pick_time1++;			MOT_DirZ=2;// 正常后退;
										Pick_DaX[Run_Num]=Run_X;
										Pick_DaY[Run_Num]=Run_Y;
										if(Pick_time1>(Pick_time_M*5))	{AuRun_Flag=3;Pick_time1=0;Apple_Pick=5;}
										break;
						case 5:
										if(AuRun_Flag==0)	{Apple_Pick=6;}	// 电机归零位;
										break;
						case 6:		// 正常05;
										Pick_time1++;			MOT_DirZ=0;
										if(MOT_Spwm<MOT_Spwm_MAX)	MOT_Spwm=MOT_Spwm+10;
										if(Pick_time1>30){Pick_time1=0;Apple_Pick=7;Run_Num++;Alarm_Ctrl();}// 果实计数;
										break;
						case 7:		// 结束采摘;归0;
										Pick_time1++;
										if(MOT_Spwm>MOT_Spwm_M)	MOT_Spwm=MOT_Spwm-10;
										if(Pick_time1>30)
										{
											Pick_time1=0;
											MOT_DirZ=0;
											MOT_Spwm=MOT_Spwm_M;
											Apple_Pick=0;
											AuRun_Flag=0;
										}
										break;
						default:break;
					}
					if(MOT_DirZ==0) MOT_Zpwm = 10000;				// 停止;
					else if(MOT_DirZ==1) MOT_Zpwm = 0;			// 前进;
					else if(MOT_DirZ==2) MOT_Zpwm = 20000;	// 后退;
					TIM_SetCompare3(TIM4,MOT_Zpwm);					// TIM4_CRR3-Z;
					TIM_SetCompare4(TIM4,MOT_Spwm);					// TIM4_CRR4;
				}
				else		AuRun_Flag=3;

				MOTOR_Set();				// 电机运行;
			}
}

void Set1_Area(void)
{
		unsigned int TmpS;
		unsigned int OldS;
		unsigned int i_S=0;
		unsigned int j_S=0;

		switch(key_val)
		{
				case 1:	if(Set_Wei>5)	Set_Wei=Set_Wei-6;		break;	// 向上;
				case 5:	if(Set_Wei<6)	Set_Wei=Set_Wei+6;		break;	// 向下;
				case 9:	if(Set_Wei>0)	Set_Wei=Set_Wei-1;		break;	// 向左;
				case 13:if(Set_Wei<11)	Set_Wei=Set_Wei+1;	break;	// 向右;
				case 15:// 0 ;Start_X  Start_Y  Stop_X  Stop_Y
								OldS = Set_Data[Set_Wei];		Set_Data[Set_Wei]=0;
								i_S = 0;	j_S = 1;
							break;
				case 10:// 1 ;
								OldS = Set_Data[Set_Wei];		Set_Data[Set_Wei]=1;
								i_S = 0;	j_S = 1;
							break;
				case 11:// 2 ;
								OldS = Set_Data[Set_Wei];		Set_Data[Set_Wei]=2;
								i_S = 0;	j_S = 1;
							break;
				case 12:// 3 ;
								OldS = Set_Data[Set_Wei];		Set_Data[Set_Wei]=3;
								i_S = 0;	j_S = 1;
							break;
				case 6:// 4 ;
								OldS = Set_Data[Set_Wei];		Set_Data[Set_Wei]=4;
								i_S = 0;	j_S = 1;
							break;
				case 7:// 5 ;
								OldS = Set_Data[Set_Wei];		Set_Data[Set_Wei]=5;
								i_S = 0;	j_S = 1;
							break;
				case 8:// 6 ;
								OldS = Set_Data[Set_Wei];		Set_Data[Set_Wei]=6;
								i_S = 0;	j_S = 1;
							break;
				case 2:// 7 ;
								OldS = Set_Data[Set_Wei];		Set_Data[Set_Wei]=7;
								i_S = 0;	j_S = 1;
							break;
				case 3:// 8 ;
								OldS = Set_Data[Set_Wei];		Set_Data[Set_Wei]=8;
								i_S = 0;	j_S = 1;
							break;
				case 4:// 9 ;
								OldS = Set_Data[Set_Wei];		Set_Data[Set_Wei]=9;
								i_S = 0;	j_S = 1;
							break;
				case 14:Area_Init();// 区域设定初始值;// D ;恢复系统设定值;
					break;
				case 16:	break;// E ;
				default:	break;
		}
		if(	j_S == 1)
		{
				if(Set_Wei<3)
				{
					TmpS = Set_Data[0]*100+Set_Data[1]*10+Set_Data[2];
					if((TmpS>SetX_max))	i_S=1;//||(TmpS>=Stop_X)
					else	Start_X = TmpS;
				}
				else if(Set_Wei<6)
				{
					TmpS = Set_Data[3]*100+Set_Data[4]*10+Set_Data[5];
					if((TmpS>SetY_max))	i_S=1;//||(TmpS>=Stop_Y)
					else	Start_Y = TmpS;
				}
				else if(Set_Wei<9)
				{
					TmpS = Set_Data[6]*100+Set_Data[7]*10+Set_Data[8];
					if((TmpS>SetX_max))	i_S=1;//||(TmpS<=Start_X)
					else	Stop_X = TmpS;
				}
				else if(Set_Wei<12)
				{
					TmpS = Set_Data[9]*100+Set_Data[10]*10+Set_Data[11];
					if((TmpS>SetY_max))	i_S=1;//||(TmpS<=Start_Y)
					else	Stop_Y = TmpS;
				}
				if(i_S==1)
				{
					Set_Data[Set_Wei] = OldS;
					Alarm_Ctrl();
				}
				else
				{
					if(Set_Wei<11)	Set_Wei=Set_Wei+1;// 往后递推;
					else Set_Wei=0;
				}
		}
		key_val=0;
		if(press_flag1==1)// 退回主界面 ;
		{
			if((Stop_X<Start_X)||(Stop_Y<Start_Y))
			{
					Alarm_Ctrl();
			}
			else
			{
				Set_flag=0;LCD_flag=1;Set_Wei=0;
				AuRun_Flag=0;// 电机无操作;
				TIM_ITConfig(TIM2, TIM_IT_Update, DISABLE);	//禁止使能时钟TIM2定时中断源中断
				TIM_Cmd(TIM2, DISABLE);											//禁止使能定时器2
			}
		}
}

void Area_Init(void)
{
	//系统设定值初始默认值;
		Start_X = SrX;			Start_Y = SrY;
		Stop_X  = SpX;			Stop_Y  = SpY;
		Set_Data[0] = SrX/100;		Set_Data[1] = SrX/10%10;
		Set_Data[2] = SrX%10;			Set_Data[3] = SrY/100;
		Set_Data[4] = SrY/10%10;	Set_Data[5] = SrY%10;
		Set_Data[6] = SpX/100;		Set_Data[7] = SpX/10%10;
		Set_Data[8] = SpX%10;			Set_Data[9] = SpY/100;
		Set_Data[10] = SpY/10%10;	Set_Data[11] = SpY%10;
}











