#ifndef __LCD12864_H__
#define __LCD12864_H__

#define LCD_RS_L            GPIO_ResetBits(GPIOG, GPIO_Pin_8)                  //RS = PG8
#define LCD_RS_H            GPIO_SetBits(GPIOG, GPIO_Pin_8)
#define LCD_RW_L            GPIO_ResetBits(GPIOG, GPIO_Pin_9)                   //RW = PG9
#define LCD_RW_H            GPIO_SetBits(GPIOG, GPIO_Pin_9) 
#define LCD_EN_L            GPIO_ResetBits(GPIOG, GPIO_Pin_10) 	            //EN = PG10
#define LCD_EN_H            GPIO_SetBits(GPIOG, GPIO_Pin_10) 
#define LCD_DataIn          P8DIR = 0x00                    //数据口方向设置为输入
#define LCD_DataOut         P8DIR = 0xff                    //数据口方向设置为输出

#define LCD_RST_L           GPIO_ResetBits(GPIOG, GPIO_Pin_11)                  // RST= PG11
#define LCD_RST_H           GPIO_SetBits(GPIOG, GPIO_Pin_11)
#define DATA_12864         	GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7

#define BSP_GPIOG_SW1         GPIO_Pin_12                       //sw1=PG12 
#define BSP_GPIOG_SW2         GPIO_Pin_13                       //sw1=PG13
#define BSP_GPIOG_SW3         GPIO_Pin_14                       //sw1=PG14
#define BSP_GPIOG_SW4         GPIO_Pin_15                       //sw1=PG15
                     
unsigned char LCD_Busy(void);
void LCD_WriteCommand(unsigned char Cmd);
void LCD_WriteData(unsigned char Dat);
void LCD_Init(void);
void LCD_Position(unsigned char X,unsigned char Y);
void LCD_Flash(void);
void  LCD_Char_Display(void);
void LCD_Clear_Screen(void);
void GPIO_12864_initout(void);
void GPIO_12864_initin(void);
void Photo_Display(const unsigned char *Bmp);
void LCD_Display_Fresh(void);
void LCD_Display_Fresh02(void);

void drawPoint2(unsigned char x,unsigned char y,unsigned char color);
void Photo_Display2(const unsigned char *Bmp);

unsigned char lcd_read_data(void);
void lcd_draw_dot(unsigned long x,unsigned long y);//???? x:0-127  y:0-63  ???128*64
void clear_dot(unsigned long x,unsigned long y);
void clear_gdram(void);

extern const unsigned char Photo1[];
#endif

