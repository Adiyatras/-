#include  "stm32f10x.h"
#include  "stm32f10x_conf.h" 
#include  "bsp.h"
#include  "LCD12864.h"
#include  "keypad.h"

/* 4X4矩阵按键
  * P7,P2都是高4位操作，P7输出，P2输入
 * */




unsigned char	key_Map[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16}; //16个按键的键值数组

void Check_Key(void)
{
	unsigned char row, col; 
	unsigned int KEY_DOUT,tmp1, tmp2;
	tmp1 = 0x0800;
	for(row=0; row<4; row++)		//行扫描
	{	
		KEY_DOUT = 0X0f00;		//输出全为1
		KEY_DOUT-= tmp1;		//依次输出一个为0
		GPIOD->ODR=((GPIOD->ODR&0xf0ff)|KEY_DOUT);
		tmp1 >>=1;
		if((GPIO_ReadInputData(GPIOD)&0xf000)<0xf000)
		//if((KEY_DIN & 0xF0) < 0xF0) //P2输入是否有一位为0
		{
			tmp2 = 0x1000;			//用于检测出哪一位为0
			for(col=0; col<4; col++)	//列扫描
			{
				if(0x00 == (GPIO_ReadInputData(GPIOD) & tmp2))	//找到等于0的列
				{
					key_val = key_Map[row*4 + col];//获取键值
					return;		//退出循环
				}
				tmp2 <<= 1;		//右移1位
			}
		}

	}
}


void Key_Event(void)
{
	unsigned int tmp;
	GPIOD->ODR=((GPIOD->ODR&0xf0ff)|0x0000);
	tmp = GPIO_ReadInputData(GPIOD);			
	if ((0x00 == key_Pressed) && ((tmp & 0xF000) < 0xF000))		//如果有键按下
	{
		key_Pressed = 1;		//按键按下标识位置位
		Delay(10);				//延时去抖
		Check_Key();			//获取键
//		key_flag    = 1;		//按键标识置位
	}
    else if ((key_Pressed == 1)&&((tmp & 0xf000) == 0xF000))	//如果按键释放
    {
        key_Pressed = 0;		//清除标识位
        key_flag    = 1;		//按键标识位置位
    }
    else
    {
       Delay(1);
    }
}







