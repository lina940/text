#ifndef __OLED_H
#define __OLED_H			  	 
#include "stm32f10x.h"
#include "stdlib.h"	    	

//-----------------4线串行模式OLED端口定义----------
#define OLED_SCLK_Clr() GPIO_ResetBits(GPIOA,GPIO_Pin_5)//CLK--PA.5
#define OLED_SCLK_Set() GPIO_SetBits(GPIOA,GPIO_Pin_5)

#define OLED_SDIN_Clr() GPIO_ResetBits(GPIOA,GPIO_Pin_7)//DIN--PA.7
#define OLED_SDIN_Set() GPIO_SetBits(GPIOA,GPIO_Pin_7)
#define OLED_SDIN_Write(x) GPIO_WriteBit(GPIOA,GPIO_Pin_7,x)

#define OLED_RST_Clr() GPIO_ResetBits(GPIOD,GPIO_Pin_12)//RES--PD.12
#define OLED_RST_Set() GPIO_SetBits(GPIOD,GPIO_Pin_12)

#define OLED_DC_Clr() GPIO_ResetBits(GPIOD,GPIO_Pin_10)//DC--PD.10
#define OLED_DC_Set() GPIO_SetBits(GPIOD,GPIO_Pin_10)
 		     
#define OLED_CS_Clr()  GPIO_ResetBits(GPIOD,GPIO_Pin_9)//CS--PD.9
#define OLED_CS_Set()  GPIO_SetBits(GPIOD,GPIO_Pin_9)

#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据

//-----------------OLED常量定义-----------------
#define SIZE 16
#define XLevelL		0x00
#define XLevelH		0x10
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0xFF 
#define X_WIDTH 	128
#define Y_WIDTH 	64	

//-----------------OLED控制用函数-----------------
void BSP_OLED_Init(void);
void BSP_OLED_WriteByte(u8 dat,u8 cmd);	    
void BSP_OLED_DisplayOn(void);
void BSP_OLED_DisplayOff(void);	   							   		    
void BSP_OLED_Clear(void);
void BSP_OLED_SetPos(u8 x, u8 y);
void BSP_OLED_ShowChar(u8 x,u8 y,u8 chr);
void BSP_OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size);
void BSP_OLED_ShowChinese(u8 x,u8 y,u8 no);
void BSP_OLED_ShowString(u8 x,u8 y, unsigned char p[]);	 
void BSP_OLED_DrawBMP(u8 x0, u8 y0,u8 x1, u8 y1,unsigned char BMP[]);


void BSP_OLED_ScrollStop(void);
void BSP_OLED_ScrollStart(void);
void BSP_OLED_ScrollH(u8 x1, u8 x2, u8 y1, u8 y2, u8 speed, u8 dir);
void BSP_OLED_ScrollHV1(u8 x1, u8 x2, u8 off, u8 speed, u8 dir);
void BSP_OLED_ScrollHV2(u8 p1, u8 p2, u8 off,	u8 speed, u8 dir,	u8 r1, u8 r2);
void BSP_OLED_ScrollHV3(u8 p1, u8 p2, u8 y1, u8 y2, u8 off, u8 spd, u8 dir,	u8 r1, u8 r2);


void BSP_OLED_DrawPoint(u8 x,u8 y,u8 t);
void BSP_OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot);
#endif  
