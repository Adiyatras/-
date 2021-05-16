#include  "stm32f10x.h"
#include  "stm32f10x_conf.h" 
#include  "bsp.h"
#include  "LCD12864.h"
#include  "math.h"
#include  "motor.h"

// 液晶显示预定义数组;
unsigned char LCD_Line[16] ={"                "};
unsigned char LCD_Start[4][16] ={"  采摘机器人    ", "工位号：00      ", "                ","按E 键进入主界面" };
unsigned char LCD_Main[4][16]  ={"1.区域设定      ", "2.手动采摘      ", "3.自动采摘      ", "4.校准          "};
unsigned char LCD_Set[4][16]   ={"起点坐标：      ", "                ", "终点坐标：      ", "                "};
unsigned char LCD_State[4][16] ={"机械手坐标：    ", "                ", "数量：          ", "压力：          "};
unsigned char LCD_Adjust[4][16]={"Max:    Min:    ", "Zt:   s         ", "Xs:   mmYs:   mm", "按E 保存并退出  "};

// 界面及显示部分;
extern unsigned char LCD_flag;// 界面号: 0=>LCD_Start; 1=>LCD_Main; 2=>LCD_Set; 3=>LCD_State; 4=>坐标点图;5=>LCD_Adjust;
extern unsigned char LCD_Ying;// LCD闪烁标志;
extern unsigned char Set_flag;// 功能: 0=>主界面; 1=>区域设定; 2=>手动采摘; 3=>自动采摘;
extern unsigned int Start_X,Start_Y,Stop_X,Stop_Y,Run_X,Run_Y;// 起点坐标, 终点坐标; 机械手坐标;
extern unsigned int Run_Num,Run_V;// 数量, 压力;
extern unsigned char Set_Wei;// 区域设定界面参数位;
extern unsigned int Pick_DaX[],Pick_DaY[];
// 参数设置;
extern unsigned int  shift_X,shift_Y;			// 自动采摘时 X,Y轴摄像头偏移量;
extern unsigned int  MOT_Spwm_MAX; 				// 	机械手舵机伸缩最大间距;max
extern unsigned int  MOT_Spwm_MIN; 				// 	机械手舵机伸缩最小间距;min
extern unsigned int  Pick_time_M;					// 	Z轴出去或回来最大延时;

void GPIO_12864_initout(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
		
	// JD: PG0-PG11, 204(P2,P3) 液晶;输出;
    GPIO_InitStructure.GPIO_Pin = DATA_12864|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11; //            
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;     //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOG, &GPIO_InitStructure);// P2-JE(8), P3-JH(4)
		
	// JD: PG12-PG15, 204(P3) 独立按键;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;	             
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // 上拉输入
    GPIO_Init(GPIOG, &GPIO_InitStructure);// P3-JH(4)
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);

}

void GPIO_12864_initin(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
		
	// JD: PG0-PG11, 204(P2) 液晶;输入;
    GPIO_InitStructure.GPIO_Pin = DATA_12864;	             
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;     //浮空输入
    GPIO_Init(GPIOG, &GPIO_InitStructure);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);

}

unsigned char LCD_Busy()
{                          
    unsigned char Result;
	GPIO_12864_initin();
    LCD_RS_L;
    LCD_RW_H;
    LCD_EN_H;
    Delay(1);
	Result=(GPIO_ReadInputData(GPIOG)&0x0080);
    LCD_EN_L;
	GPIO_12864_initout(); 
    return Result; 
}

void LCD_WriteCommand(unsigned char Cmd)
{                          
    while (LCD_Busy());
    LCD_RS_L;
    LCD_RW_L;
    LCD_EN_L;
	Delay(1); 
	GPIOG->ODR=((GPIOG->ODR&0xff00)|Cmd);
    Delay(1); 
    LCD_EN_H;
    Delay(1); 
    LCD_EN_L;  
}

void LCD_WriteData(unsigned char Dat)
{                          
    while (LCD_Busy());
    LCD_RS_H;
    LCD_RW_L;
    LCD_EN_L;
	GPIOG->ODR=((GPIOG->ODR&0xff00)|Dat);
    Delay(1); 
    LCD_EN_H;
    Delay(1); 
    LCD_EN_L; 
}

void LCD_Init()
{ 
    LCD_RST_L;		                                    //液晶复位
    Delay(3);                    
    LCD_RST_H;      
    Delay(3); 
    
    LCD_WriteCommand(0x34);                                 //扩充指令操作
    Delay(5); 
    LCD_WriteCommand(0x30);                                 //基本指令操作
    Delay(5); 
    LCD_WriteCommand(0x0C);                                 //显示开，关光标
    Delay(5); 
    LCD_WriteCommand(0x01);                                 //清除LCD的显示内容
    Delay(5); 
	
}

void LCD_Position(unsigned char X,unsigned char Y)
{                          
   unsigned char  Pos;
   if (X==1)      X=0x80;
      
   else if (X==2) X=0x90;
     
   else if (X==3) X=0x88;
     
   else if (X==4) X=0x98;
      
   Pos = X+Y ; 
 
   LCD_WriteCommand(Pos);                                   //显示地址
}

void LCD_Flash() 
{
   LCD_WriteCommand(0x08);                                  // close display
   Delay(400); 
   LCD_WriteCommand(0x0c);                                  // open display  
   Delay(400); 
   LCD_WriteCommand(0x08);   
   Delay(400); 
   LCD_WriteCommand(0x0c);   
   Delay(400); 
   LCD_WriteCommand(0x08);   
   Delay(200);  
   LCD_WriteCommand(0x0c);   
   Delay(5); 
   LCD_WriteCommand(0x01);                                  // clear display
   Delay(5); 
}

void  LCD_Char_Display()
{
   unsigned char  s;
   LCD_Clear_Screen();                                      //清屏    
   LCD_WriteCommand(0x80);                                  //设置显示位置为第一行  
   for(s=0;s<16;s++)
   {
     LCD_WriteData(0x30+s); 
   }
   LCD_WriteCommand(0x90);                                  //设置显示位置为第二行 
   for(s=0;s<16;s++)
   {
     LCD_WriteData(0x40+s); 
   }
   LCD_WriteCommand(0x88);                                  //设置显示位置为第三行  
   for(s=0;s<16;s++)
   {
     LCD_WriteData(0x50+s); 
   }
   LCD_WriteCommand(0x98);                                  //设置显示位置为第四行   
   for(s=0;s<16;s++)
   {
     LCD_WriteData(0x60+s); 
   }
}

void Photo_Display(const unsigned char *Bmp)	
{ 
  unsigned char i,j;

  LCD_WriteCommand(0x34);                                   //写数据时,关闭图形显示

  for(i=0;i<32;i++)
  {
    LCD_WriteCommand(0x80+i);                               //先写入水平坐标值
    LCD_WriteCommand(0x80);                                 //写入垂直坐标值
    for(j=0;j<16;j++)                                       //再写入两个8位元的数据    
    LCD_WriteData(*Bmp++);     
    Delay(1); 
  }

  for(i=0;i<32;i++)
  { 
    LCD_WriteCommand(0x80+i);
    LCD_WriteCommand(0x88);
    for(j=0;j<16;j++)         
    LCD_WriteData(*Bmp++);    
    Delay(1); 
  }
  LCD_WriteCommand(0x36);                                   //写完数据,开图形显示 
}

void LCD_Clear_Screen()
{
   LCD_WriteCommand(0x34);                                  //扩充指令操作
   Delay(5);    
   LCD_WriteCommand(0x30);                                  //基本指令操作
   Delay(5);   
   LCD_WriteCommand(0x01);                                  //清屏 
   Delay(5);       
}

unsigned char lcd_read_data(void)
{ 
	unsigned char r_data;   
	GPIO_12864_initin();     
	LCD_RS_H;     
	LCD_RW_H; 
  LCD_EN_L;  
  LCD_EN_H;  
  Delay(1);
  r_data = (GPIO_ReadInputData(GPIOG)&0xff);   
  Delay(1);      
  LCD_EN_L;
  GPIO_12864_initout();  
  return r_data;   
}

void lcd_draw_dot(unsigned long x,unsigned long y)//???? x:0-127  y:0-63  ???128*64
{
	  unsigned long x_add,y_add,x_move;
	  unsigned char temph,templ; 
	  LCD_WriteCommand(0x34);
	  LCD_WriteCommand(0x36);
	  if(y<=31)
		{
			y_add=y;
			x_add=x/16;
		}
		else if(y>31&&y<=63)
		{
			y_add=y-32;
			x_add=x/16+8;
		}
	  if(x<(x_add%8)*16+8)
		{
			x_move=(((x_add%8)*16)+8)-x-1;
			LCD_WriteCommand(y_add+0x80);    
      LCD_WriteCommand(x_add+0x80);
			lcd_read_data();
			temph=lcd_read_data();
			templ=lcd_read_data();
			LCD_WriteCommand(y_add+0x80);    
      LCD_WriteCommand(x_add+0x80);
			LCD_WriteData((0x01<<x_move)|temph);
	    LCD_WriteData(templ);	
		}
		else
		{
		  x_move=x-(((x_add%8)*16)+8);
		  LCD_WriteCommand(y_add+0x80);    
      LCD_WriteCommand(x_add+0x80);
			lcd_read_data();
			temph=lcd_read_data();
			templ=lcd_read_data();
			LCD_WriteCommand(y_add+0x80);    
      LCD_WriteCommand(x_add+0x80);
			LCD_WriteData(temph);
	    LCD_WriteData((0x80>>x_move)|templ);
		}		
}

void clear_dot(unsigned long x,unsigned long y)
{
		unsigned long x_add,y_add,x_move;
	  unsigned char temph,templ; 
	  LCD_WriteCommand(0x34);
	  LCD_WriteCommand(0x36);
	  if(y<=31)
		{
			y_add=y;
			x_add=x/16;
		}
		else if(y>31&&y<=63)
		{
			y_add=y-32;
			x_add=x/16+8;
		}
	  if(x<(x_add%8)*16+8)
		{
			x_move=(((x_add%8)*16)+8)-x-1;
			LCD_WriteCommand(y_add+0x80);    
      LCD_WriteCommand(x_add+0x80);
			lcd_read_data();
			temph=lcd_read_data();
			templ=lcd_read_data();
			LCD_WriteCommand(y_add+0x80);    
      LCD_WriteCommand(x_add+0x80);
			LCD_WriteData((~(0x01<<x_move))&temph);
	    LCD_WriteData(templ);	
		}
		else
		{
		  x_move=x-(((x_add%8)*16)+8);
		  LCD_WriteCommand(y_add+0x80);    
      LCD_WriteCommand(x_add+0x80);
			lcd_read_data();
			temph=lcd_read_data();
			templ=lcd_read_data();
			LCD_WriteCommand(y_add+0x80);    
      LCD_WriteCommand(x_add+0x80);
			LCD_WriteData(temph);
	    LCD_WriteData((~(0x80>>x_move))&templ);
		}	 
}

void clear_gdram(void)
{
    unsigned char x,y ;
	  LCD_WriteCommand(0x34);
	  LCD_WriteCommand(0x36);
	  for(x=0;x<16;x++)
      for(y=0;y<32;y++)
      {
				LCD_WriteCommand(y+0x80);
				LCD_WriteCommand(x+0x80);
        LCD_WriteData(0x00);
				LCD_WriteData(0x00);
      }	
}



void LCD_Display_Fresh(void)
{
	u16 i=0;
	u16 datax,datay;
	switch(LCD_flag)
	{
		case 0://开机界面
			LCD_Position(1,0);
			for(i=0;i<16;i++)	LCD_WriteData(LCD_Start[0][i]);
			 
			LCD_Position(2,0);
			for(i=0;i<16;i++)	LCD_WriteData(LCD_Start[1][i]);
			
			LCD_Position(3,0); 
			for(i=0;i<16;i++)	LCD_WriteData(LCD_Start[2][i]);
			
			LCD_Position(4,0);
			if(LCD_Ying)
			for(i=0;i<16;i++)	LCD_WriteData(LCD_Start[3][i]);
			else
			for(i=0;i<16;i++)	LCD_WriteData(LCD_Line[i]);
			break;
		case 1://主界面
			LCD_Position(1,0);
			for(i=0;i<16;i++)	LCD_WriteData(LCD_Main[0][i]);
			 
			LCD_Position(2,0);
			for(i=0;i<16;i++)	LCD_WriteData(LCD_Main[1][i]);
			
			LCD_Position(3,0); 
			for(i=0;i<16;i++)	LCD_WriteData(LCD_Main[2][i]);
			
			LCD_Position(4,0);
			for(i=0;i<16;i++)	LCD_WriteData(LCD_Main[3][i]);
			break;
		case 2://区域设定界面
			if(LCD_Ying)
			{
				LCD_Position(1,0);
				for(i=0;i<16;i++)	LCD_WriteData(LCD_Set[0][i]);
				 
				LCD_Position(2,0);
				for(i=0;i<2;i++)	LCD_WriteData(LCD_Set[1][i]);
				LCD_WriteData(0x30+Start_X/100);
				LCD_WriteData(0x30+Start_X/10%10);
				LCD_WriteData(0x30+Start_X%10);
				LCD_WriteData('x');
				LCD_WriteData(0x30+Start_Y/100);
				LCD_WriteData(0x30+Start_Y/10%10);
				LCD_WriteData(0x30+Start_Y%10);
				for(i=9;i<16;i++)	LCD_WriteData(LCD_Set[1][i]);
				
				LCD_Position(3,0); 
				for(i=0;i<16;i++)	LCD_WriteData(LCD_Set[2][i]);
				
				LCD_Position(4,0);
				for(i=0;i<2;i++)	LCD_WriteData(LCD_Set[3][i]);
					LCD_WriteData(0x30+Stop_X/100);
					LCD_WriteData(0x30+Stop_X/10%10);
					LCD_WriteData(0x30+Stop_X%10);
					LCD_WriteData('x');
					LCD_WriteData(0x30+Stop_Y/100);
					LCD_WriteData(0x30+Stop_Y/10%10);
					LCD_WriteData(0x30+Stop_Y%10);
				for(i=9;i<16;i++)	LCD_WriteData(LCD_Set[3][i]);
			}
			else
			{
				LCD_Position(1,0);
				for(i=0;i<16;i++)	LCD_WriteData(LCD_Set[0][i]);
				 
				LCD_Position(2,0);
				for(i=0;i<2;i++)	LCD_WriteData(LCD_Set[1][i]);
				switch(Set_Wei)
				{
					case 0:
					LCD_WriteData(' ');
					LCD_WriteData(0x30+Start_X/10%10);
					LCD_WriteData(0x30+Start_X%10);
					LCD_WriteData('x');
					LCD_WriteData(0x30+Start_Y/100);
					LCD_WriteData(0x30+Start_Y/10%10);
					LCD_WriteData(0x30+Start_Y%10);
					break;
					case 1:
					LCD_WriteData(0x30+Start_X/100);
					LCD_WriteData(' ');
					LCD_WriteData(0x30+Start_X%10);
					LCD_WriteData('x');
					LCD_WriteData(0x30+Start_Y/100);
					LCD_WriteData(0x30+Start_Y/10%10);
					LCD_WriteData(0x30+Start_Y%10);
					break;
					case 2:
					LCD_WriteData(0x30+Start_X/100);
					LCD_WriteData(0x30+Start_X/10%10);
					LCD_WriteData(' ');
					LCD_WriteData('x');
					LCD_WriteData(0x30+Start_Y/100);
					LCD_WriteData(0x30+Start_Y/10%10);
					LCD_WriteData(0x30+Start_Y%10);
					break;
					case 3:
					LCD_WriteData(0x30+Start_X/100);
					LCD_WriteData(0x30+Start_X/10%10);
					LCD_WriteData(0x30+Start_X%10);
					LCD_WriteData('x');
					LCD_WriteData(' ');
					LCD_WriteData(0x30+Start_Y/10%10);
					LCD_WriteData(0x30+Start_Y%10);
					break;
					case 4:
					LCD_WriteData(0x30+Start_X/100);
					LCD_WriteData(0x30+Start_X/10%10);
					LCD_WriteData(0x30+Start_X%10);
					LCD_WriteData('x');
					LCD_WriteData(0x30+Start_Y/100);
					LCD_WriteData(' ');
					LCD_WriteData(0x30+Start_Y%10);
					break;
					case 5:
					LCD_WriteData(0x30+Start_X/100);
					LCD_WriteData(0x30+Start_X/10%10);
					LCD_WriteData(0x30+Start_X%10);
					LCD_WriteData('x');
					LCD_WriteData(0x30+Start_Y/100);
					LCD_WriteData(0x30+Start_Y/10%10);
					LCD_WriteData(' ');
					break;
					default:
				LCD_WriteData(0x30+Start_X/100);
				LCD_WriteData(0x30+Start_X/10%10);
				LCD_WriteData(0x30+Start_X%10);
				LCD_WriteData('x');
				LCD_WriteData(0x30+Start_Y/100);
				LCD_WriteData(0x30+Start_Y/10%10);
				LCD_WriteData(0x30+Start_Y%10);
						break;
				}
				for(i=9;i<16;i++)	LCD_WriteData(LCD_Set[1][i]);
				
				LCD_Position(3,0); 
				for(i=0;i<16;i++)	LCD_WriteData(LCD_Set[2][i]);
				
				LCD_Position(4,0);
				for(i=0;i<2;i++)	LCD_WriteData(LCD_Set[3][i]);
				switch(Set_Wei)
				{
					case 6:
					LCD_WriteData(' ');
					LCD_WriteData(0x30+Stop_X/10%10);
					LCD_WriteData(0x30+Stop_X%10);
					LCD_WriteData('x');
					LCD_WriteData(0x30+Stop_Y/100);
					LCD_WriteData(0x30+Stop_Y/10%10);
					LCD_WriteData(0x30+Stop_Y%10);
					break;
					case 7:
					LCD_WriteData(0x30+Stop_X/100);
					LCD_WriteData(' ');
					LCD_WriteData(0x30+Stop_X%10);
					LCD_WriteData('x');
					LCD_WriteData(0x30+Stop_Y/100);
					LCD_WriteData(0x30+Stop_Y/10%10);
					LCD_WriteData(0x30+Stop_Y%10);
					break;
					case 8:
					LCD_WriteData(0x30+Stop_X/100);
					LCD_WriteData(0x30+Stop_X/10%10);
					LCD_WriteData(' ');
					LCD_WriteData('x');
					LCD_WriteData(0x30+Stop_Y/100);
					LCD_WriteData(0x30+Stop_Y/10%10);
					LCD_WriteData(0x30+Stop_Y%10);
					break;
					case 9:
					LCD_WriteData(0x30+Stop_X/100);
					LCD_WriteData(0x30+Stop_X/10%10);
					LCD_WriteData(0x30+Stop_X%10);
					LCD_WriteData('x');
					LCD_WriteData(' ');
					LCD_WriteData(0x30+Stop_Y/10%10);
					LCD_WriteData(0x30+Stop_Y%10);
					break;
					case 10:
					LCD_WriteData(0x30+Stop_X/100);
					LCD_WriteData(0x30+Stop_X/10%10);
					LCD_WriteData(0x30+Stop_X%10);
					LCD_WriteData('x');
					LCD_WriteData(0x30+Stop_Y/100);
					LCD_WriteData(' ');
					LCD_WriteData(0x30+Stop_Y%10);
					break;
					case 11:
					LCD_WriteData(0x30+Stop_X/100);
					LCD_WriteData(0x30+Stop_X/10%10);
					LCD_WriteData(0x30+Stop_X%10);
					LCD_WriteData('x');
					LCD_WriteData(0x30+Stop_Y/100);
					LCD_WriteData(0x30+Stop_Y/10%10);
					LCD_WriteData(' ');
					break;
					default:
				LCD_WriteData(0x30+Stop_X/100);
				LCD_WriteData(0x30+Stop_X/10%10);
				LCD_WriteData(0x30+Stop_X%10);
				LCD_WriteData('x');
				LCD_WriteData(0x30+Stop_Y/100);
				LCD_WriteData(0x30+Stop_Y/10%10);
				LCD_WriteData(0x30+Stop_Y%10);
						break;
				}
				for(i=9;i<16;i++)	LCD_WriteData(LCD_Set[3][i]);
			}
			break;
		case 3://实时状态界面
			LCD_Position(1,0);
			for(i=0;i<16;i++)	LCD_WriteData(LCD_State[0][i]);
			 
			LCD_Position(2,0);
			for(i=0;i<2;i++)	LCD_WriteData(LCD_State[1][i]);
			LCD_WriteData(0x30+Run_X/100);
			LCD_WriteData(0x30+Run_X/10%10);
			LCD_WriteData(0x30+Run_X%10);
			LCD_WriteData('x');
			LCD_WriteData(0x30+Run_Y/100);
			LCD_WriteData(0x30+Run_Y/10%10);
			LCD_WriteData(0x30+Run_Y%10);
			for(i=9;i<16;i++)	LCD_WriteData(LCD_State[1][i]);
			
			LCD_Position(3,0); 
			for(i=0;i<6;i++)	LCD_WriteData(LCD_State[2][i]);
			LCD_WriteData(0x30+Run_Num/10);
			LCD_WriteData(0x30+Run_Num%10);
			for(i=8;i<16;i++)	LCD_WriteData(LCD_State[2][i]);
			
			LCD_Position(4,0);
			for(i=0;i<6;i++)	LCD_WriteData(LCD_State[3][i]);
			LCD_WriteData(0x30+Run_V/100);
			LCD_WriteData('.');
			LCD_WriteData(0x30+Run_V/10%10);
			LCD_WriteData(0x30+Run_V%10);
			LCD_WriteData('V');
			for(i=11;i<16;i++)	LCD_WriteData(LCD_State[3][i]);
			break;
		case 4://自动采摘结果界面
				LCD_Clear_Screen();	   //清屏函数 防止乱码
				clear_gdram();
				for(i=0;i<Run_Num;i++)
				{
					datax = Pick_DaX[i]/5+2;
					datay = 61-Pick_DaY[i]/5;
					lcd_draw_dot(datax,datay);
					if(datax < 127)	
					lcd_draw_dot(datax+1,datay);
					if(datax > 0)	
					lcd_draw_dot(datax-1,datay);
					if(datay < 127)	
					lcd_draw_dot(datax,datay+1);
					if(datay > 0)
					lcd_draw_dot(datax,datay-1);
				}
				LCD_flag=6;
			break;
		case 5://校准界面
			if(LCD_Ying)
			{
				LCD_Position(1,0);	// "max:    min:    ",
				for(i=0;i<4;i++)	LCD_WriteData(LCD_Adjust[0][i]);
				LCD_WriteData(0x30+MOT_Spwm_MAX/1000);
				LCD_WriteData(0x30+MOT_Spwm_MAX/100%10);
				LCD_WriteData(0x30+MOT_Spwm_MAX/10%10);
				LCD_WriteData(0x30+MOT_Spwm_MAX%10);
				for(i=8;i<12;i++)	LCD_WriteData(LCD_Adjust[0][i]);
				LCD_WriteData(0x30+MOT_Spwm_MIN/1000);
				LCD_WriteData(0x30+MOT_Spwm_MIN/100%10);
				LCD_WriteData(0x30+MOT_Spwm_MIN/10%10);
				LCD_WriteData(0x30+MOT_Spwm_MIN%10);
				 
				LCD_Position(2,0);	//  "Zt:   s         ", 
				for(i=0;i<4;i++)	LCD_WriteData(LCD_Adjust[1][i]);
				if((Pick_time_M/10)==0)
						LCD_WriteData(' ');
				else
						LCD_WriteData(0x30+Pick_time_M/10);
				LCD_WriteData(0x30+Pick_time_M%10);
				for(i=6;i<16;i++)	LCD_WriteData(LCD_Adjust[1][i]);
				
				LCD_Position(3,0);	// 	"Xs:   mmYs:   mm", 
				for(i=0;i<3;i++)	LCD_WriteData(LCD_Adjust[2][i]);
				if((shift_X/100)==0)
						LCD_WriteData(' ');
				else
						LCD_WriteData(0x30+shift_X/100);
				LCD_WriteData(0x30+shift_X/10%10);
				LCD_WriteData(0x30+shift_X%10);
				for(i=6;i<11;i++)	LCD_WriteData(LCD_Adjust[2][i]);
				if((shift_Y/100)==0)
						LCD_WriteData(' ');
				else
						LCD_WriteData(0x30+shift_Y/100);
				LCD_WriteData(0x30+shift_Y/10%10);
				LCD_WriteData(0x30+shift_Y%10);
				for(i=14;i<16;i++)	LCD_WriteData(LCD_Adjust[2][i]);
				
				LCD_Position(4,0);	//  "按E 保存校准参数"
				for(i=0;i<16;i++)	LCD_WriteData(LCD_Adjust[3][i]);
			}
			else
			{
				LCD_Position(1,0);	// "max:    min:    ",
				for(i=0;i<4;i++)	LCD_WriteData(LCD_Adjust[0][i]);
					if(Set_Wei == 1)// max;
					{
						LCD_WriteData(' ');
						LCD_WriteData(' ');
						LCD_WriteData(' ');
						LCD_WriteData(' ');
					}
					else
					{
						LCD_WriteData(0x30+MOT_Spwm_MAX/1000);
						LCD_WriteData(0x30+MOT_Spwm_MAX/100%10);
						LCD_WriteData(0x30+MOT_Spwm_MAX/10%10);
						LCD_WriteData(0x30+MOT_Spwm_MAX%10);
					}
				for(i=8;i<12;i++)	LCD_WriteData(LCD_Adjust[0][i]);
					if(Set_Wei == 2)// min;
					{
						LCD_WriteData(' ');
						LCD_WriteData(' ');
						LCD_WriteData(' ');
						LCD_WriteData(' ');
					}
					else
					{
						LCD_WriteData(0x30+MOT_Spwm_MIN/1000);
						LCD_WriteData(0x30+MOT_Spwm_MIN/100%10);
						LCD_WriteData(0x30+MOT_Spwm_MIN/10%10);
						LCD_WriteData(0x30+MOT_Spwm_MIN%10);
					}
				 
				LCD_Position(2,0);	//  "Zt:   s         ", 
				for(i=0;i<4;i++)	LCD_WriteData(LCD_Adjust[1][i]);
					if(Set_Wei == 3)// Z time;
					{
						LCD_WriteData(' ');
						LCD_WriteData(' ');
					}
					else
					{
						if((Pick_time_M/10)==0)
								LCD_WriteData(' ');
						else
								LCD_WriteData(0x30+Pick_time_M/10);
						LCD_WriteData(0x30+Pick_time_M%10);
					}
				for(i=6;i<16;i++)	LCD_WriteData(LCD_Adjust[1][i]);
				
				LCD_Position(3,0);	//  "Xs:   mmYs:    mm",
				for(i=0;i<3;i++)	LCD_WriteData(LCD_Adjust[2][i]);
					if(Set_Wei == 4)// X;
					{
						LCD_WriteData(' ');
						LCD_WriteData(' ');
						LCD_WriteData(' ');
					}
					else
					{
						if((shift_X/100)==0)
								LCD_WriteData(' ');
						else
								LCD_WriteData(0x30+shift_X/100);
						LCD_WriteData(0x30+shift_X/10%10);
						LCD_WriteData(0x30+shift_X%10);
					}
				for(i=6;i<11;i++)	LCD_WriteData(LCD_Adjust[2][i]);
					if(Set_Wei == 5)// Y;
					{
						LCD_WriteData(' ');
						LCD_WriteData(' ');
						LCD_WriteData(' ');
					}
					else
					{
						if((shift_Y/100)==0)
								LCD_WriteData(' ');
						else
								LCD_WriteData(0x30+shift_Y/100);
						LCD_WriteData(0x30+shift_Y/10%10);
						LCD_WriteData(0x30+shift_Y%10);
					}
				for(i=14;i<16;i++)	LCD_WriteData(LCD_Adjust[2][i]);
				
				LCD_Position(4,0);	//  "按E 保存并退出  "
					for(i=0;i<16;i++)	LCD_WriteData(LCD_Adjust[3][i]);
			}
			break;
		default:	break;
	}

}	

