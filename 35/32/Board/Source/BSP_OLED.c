/*
*******************************************************************
*    模块：BSP_OLED 
*    描述：OLED模块驱动
*    作者：Shao
*    时间：2018.07.05
*    版本：V1.0.0
			OLED的显存存放格式如下.
			[0]0 1 2 3 ... 127	
			[1]0 1 2 3 ... 127	
			[2]0 1 2 3 ... 127	
			[3]0 1 2 3 ... 127	
			[4]0 1 2 3 ... 127	
			[5]0 1 2 3 ... 127	
			[6]0 1 2 3 ... 127	
			[7]0 1 2 3 ... 127 			
********************************************************************
*/
#include "BSP_Delay.h"
#include "BSP_OLED.h"
#include "BSP_OLEDFont.h"  	 

#if OLED_INTERFACE == 1

static void BSP_OLED_GPIO_Init(void){
	GPIO_InitTypeDef  GPIO_InitStructure;
 	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //使能A端口时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_7;	 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
 	GPIO_Init(GPIOA, &GPIO_InitStructure);	  				//初始化GPIOA5,7
 	GPIO_SetBits(GPIOA,GPIO_Pin_5|GPIO_Pin_7);				//初始都输出高
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);	 //使能D端口时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_10|GPIO_Pin_9;	 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 	//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
 	GPIO_Init(GPIOD, &GPIO_InitStructure);	  				//初始化GPIOD9,10,12
 	GPIO_SetBits(GPIOD,GPIO_Pin_12|GPIO_Pin_10|GPIO_Pin_9);		
}

/*****************************************************************
*	函 数 名: BSP_OLED_WriteByte
*	功能说明: 向SSD1106写入一个字节，先高位，再低位
*	形    参: dat:要写入的数据/命令
						cmd:数据/命令标志 0,表示命令;1,表示数据
*	返 回 值: 无
*****************************************************************/
void BSP_OLED_WriteByte(u8 dat, u8 cmd)
{	
	u8 i;			  
	if(cmd)  //数据
	  OLED_DC_Set();
	else     //命令
	  OLED_DC_Clr();	
	
	OLED_CS_Clr();
	for(i=0;i<8;i++){			  
		OLED_SCLK_Clr();
		if(dat&0x80)
			OLED_SDIN_Set();
		else 
			OLED_SDIN_Clr();
		OLED_SCLK_Set();
		dat<<=1;   
	}	
	OLED_CS_Set();
	OLED_DC_Set();
} 

#else   //I2C
static void BSP_OLED_GPIO_Init(void){
	GPIO_InitTypeDef  GPIO_InitStructure; 	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //使能A端口时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_7;	 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; 		
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
 	GPIO_Init(GPIOA, &GPIO_InitStructure);	  				//初始化GPIOA5,7
 	GPIO_SetBits(GPIOA,GPIO_Pin_5|GPIO_Pin_7);				//初始都输出高
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);	 //使能D端口时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_10|GPIO_Pin_9;	 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;	 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; 		
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//速度50MHz
 	GPIO_Init(GPIOD, &GPIO_InitStructure);	  					//初始化GPIOD12
 	GPIO_ResetBits(GPIOD,GPIO_Pin_12|GPIO_Pin_10|GPIO_Pin_9);		
}

static void OLED_I2C_Delay(u32 t){
	while(t--);	
}

static void OLED_I2C_Start(void){
	OLED_SDA_Set();	
	OLED_SCL_Set();	
	OLED_I2C_Delay(10);
	OLED_SDA_Clr();
	OLED_I2C_Delay(10);	
	OLED_SCL_Clr();	  //准备收发数据
}

static void OLED_I2C_Stop(void){	
	OLED_SCL_Clr();
	OLED_SDA_Clr();	
	OLED_I2C_Delay(10);
	OLED_SCL_Set();
	OLED_SDA_Set();  //总线结束信号
	OLED_I2C_Delay(10);	
}

static int OLED_I2C_WaitAck(void){
	u32 t=0;
	
	OLED_SCL_Clr();
	OLED_SDA_Set();
	OLED_I2C_Delay(10);
	OLED_SCL_Set();
	OLED_I2C_Delay(10);
	
	while(OLED_SDA_In()){
		t++;
		if(t>=100){
			OLED_I2C_Stop();			
			return -1;
		}
		OLED_I2C_Delay(10);	
	}
	OLED_SCL_Clr();
	return 0;
}

static void OLED_I2C_ACK(void){
	OLED_SCL_Clr();
	OLED_SDA_Clr();
	OLED_I2C_Delay(10);
	OLED_SCL_Set();
	OLED_I2C_Delay(10);
	OLED_SCL_Clr();
}

static void OLED_I2C_NACK(void){
	OLED_SCL_Clr();
	OLED_SDA_Set();
	OLED_I2C_Delay(10);
	OLED_SCL_Set();
	OLED_I2C_Delay(10);
	OLED_SCL_Clr();
}

static void BSP_OLED_WriteByte0(u8 dat)
{	
	u8 i;	
	for(i=0;i<8;i++){			  
		OLED_SCL_Clr();		
		if(dat&0x80)
			OLED_SDA_Set();
		else 
			OLED_SDA_Clr();
		OLED_I2C_Delay(5);
		OLED_SCL_Set();
		OLED_I2C_Delay(5);
		dat<<=1;   		
	}	
	OLED_I2C_WaitAck();	
} 

static u8 BSP_OLED_ReadByte0(u8 ack)
{	
	u8 i, dat=0;
	//OLED_SCL_Set();
	for(i=0;i<8;i++){			  
		OLED_SCL_Clr();
		OLED_I2C_Delay(10);
		OLED_SCL_Set();
		OLED_I2C_Delay(10);
		dat <<= 1;
		if(OLED_SDA_In())
			dat++;
	}	
	if(ack)
		OLED_I2C_ACK();
	else
		OLED_I2C_NACK();
	return dat;	
} 


void BSP_OLED_WriteByte(u8 data, u8 cmd){
	OLED_I2C_Start();
	BSP_OLED_WriteByte0(OLED_WRITE_ADDR);
	if(cmd) //data
		BSP_OLED_WriteByte0(0x40);
	else  //cmd
		BSP_OLED_WriteByte0(0x00);
	BSP_OLED_WriteByte0(data);
	OLED_I2C_Stop();
}

//发送若干字节数据
void BSP_OLED_WriteNBytes(u8 *data, u16 len){
	u16 i=0;
	OLED_I2C_Start();
	BSP_OLED_WriteByte0(OLED_WRITE_ADDR);
	BSP_OLED_WriteByte0(0x40);
	for(i=0;i<len;i++)
		BSP_OLED_WriteByte0(data[i]);
	OLED_I2C_Stop();
}

#endif


/********************************************************************
*	函 数 名: BSP_OLED_DisplayOn
*	功能说明: 开启OLED显示
*	形    参: 无
*	返 回 值: 无
*********************************************************************/
void BSP_OLED_DisplayOn(void)
{
	BSP_OLED_WriteByte(0x8D,OLED_CMD);  //0x14打开电荷泵
	BSP_OLED_WriteByte(0x14,OLED_CMD);
	BSP_OLED_WriteByte(0xAF,OLED_CMD);  //打开显示
} 

/********************************************************************
*	函 数 名: BSP_OLED_DisplayOff
*	功能说明: 关闭OLED显示
*	形    参: 无
*	返 回 值: 无
********************************************************************/
void BSP_OLED_DisplayOff(void)
{
	BSP_OLED_WriteByte(0x8D,OLED_CMD);  //0x10关闭电荷泵
	BSP_OLED_WriteByte(0x10,OLED_CMD);
	BSP_OLED_WriteByte(0xAE,OLED_CMD);  //关闭显示
}		   	

/*******************************************************************
*	函 数 名: BSP_OLED_Clear
*	功能说明: 清屏函数，清屏过后，屏幕变黑
*	形    参: 无
*	返 回 值: 无
********************************************************************/
void BSP_OLED_Clear(void)  
{  
	u8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		BSP_OLED_WriteByte(0xb0+i,OLED_CMD);    //设置页地址（0~7）
		BSP_OLED_WriteByte(0x00,OLED_CMD);      //设置显示位置—列低地址
		BSP_OLED_WriteByte(0x10,OLED_CMD);      //设置显示位置—列高地址   
		for(n=0;n<128;n++)
			BSP_OLED_WriteByte(0,OLED_DATA); 
	}
}

void BSP_OLED_All(void)  
{  
	u8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		BSP_OLED_WriteByte(0xb0+i,OLED_CMD);    //设置页地址（0~7）
		BSP_OLED_WriteByte(0x00,OLED_CMD);      //设置显示位置—列低地址
		BSP_OLED_WriteByte(0x10,OLED_CMD);      //设置显示位置—列高地址   
		for(n=0;n<128;n++)
			BSP_OLED_WriteByte(0xff,OLED_DATA); 
	}
}

/****************************************************************
*	函 数 名: BSP_OLED_SetPos
*	功能说明: 设置屏幕坐标
*	形    参: u8 x:横坐标，列号
						u8 p:纵坐标，页号
*	返 回 值: 无
******************************************************************/
void BSP_OLED_SetPos(u8 x, u8 p) 
{ 
	BSP_OLED_WriteByte(0xb0+p,OLED_CMD);
	BSP_OLED_WriteByte(((x&0xf0)>>4)|0x10,OLED_CMD);
	BSP_OLED_WriteByte(x&0x0f,OLED_CMD); 
}   	  

/******************************************************************
*	函 数 名: BSP_OLED_ShowChar
*	功能说明: 在指定位置显示一个字符,包括部分字符
*	形    参: x:0~127	p:0~7 页号	chr:字符
*	返 回 值: 无
*******************************************************************/
void BSP_OLED_ShowChar(u8 x,u8 p,u8 chr)
{      	
	unsigned char i, c = chr-' '; //得到chr偏移后的值			
	if(x>Max_Column-1)	{ x=0;	p=p+2;}
	if(SIZE ==16){
		BSP_OLED_SetPos(x,p);	
		for(i=0;i<8;i++)
			BSP_OLED_WriteByte(F8X16[c*16+i],OLED_DATA);
		BSP_OLED_SetPos(x, p+1);
		for(i=0;i<8;i++)
			BSP_OLED_WriteByte(F8X16[c*16+i+8],OLED_DATA);
	}
	else {	
		BSP_OLED_SetPos(x,p+1);
		for(i=0;i<6;i++)
			BSP_OLED_WriteByte(F6x8[c][i],OLED_DATA);
	}
}

/************************************************************************
*	函 数 名: BSP_OLed_Pow
*	功能说明: m^n函数
*	形    参: u8 m;u8 n
*	返 回 值: 无
*************************************************************************/
static u32 BSP_OLED_Pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}				  

/***********************************************************************
*	函 数 名: BSP_OLED_ShowNum
*	功能说明: 显示2个数字函数
*	形    参: (x,y): 起点坐标，x-列号0~127, p:页号0~7
*           num: 数值(0~4294967295);						
*           len: 数字的位数
*			size: 字体大小，size=16表示宽度为16，size=32，表示宽度为32
*	返 回 值: 无
***********************************************************************/
void BSP_OLED_ShowNum(u8 x, u8 p,u32 num,u8 len,u8 size)
{         	
	u8 t,temp, enshow=0;						   
	for(t=0; t<len; t++){
		temp = (num / BSP_OLED_Pow(10,len-t-1)) % 10;
		if(enshow == 0 && t < (len-1)){
			if(temp==0){
				BSP_OLED_ShowChar(x+(size/2)*t, p, ' ');
				continue;
			}
			else enshow=1; 
		}
	 	BSP_OLED_ShowChar(x+(size/2)*t, p ,temp+'0'); 
	}
} 

/**********************************************************************
*	函 数 名: BSP_OLED_ShowString
*	功能说明: 显示一个字符号串
*	形    参: x,y:起点坐标,x-列号0~127, y:页号0~7 	char:字符
*	返 回 值: 无
**********************************************************************/
void BSP_OLED_ShowString(u8 x,u8 p,unsigned char chr[])
{
	unsigned char j=0;
	while (chr[j]!='\0')
	{		
		BSP_OLED_ShowChar(x,p,chr[j]);
		x+=8; 
		if(x>120){x=0; p+=2;}
		j++;
	}
}

/********************************************************************
*	函 数 名: BSP_OLED_ShowChinese
*	功能说明: 显示16*16汉字
*	形    参: x, y: 起点坐标 x-列号0~127, y:页号0~7		no: 汉字编号
*	返 回 值: 无
*********************************************************************/
void BSP_OLED_ShowChinese(u8 x,u8 y,u8 no)
{      			    
	u8 t;
	BSP_OLED_SetPos(x,y);	
	for(t=0;t<16;t++)
		BSP_OLED_WriteByte(Hzk[2*no][t],OLED_DATA);	
	BSP_OLED_SetPos(x,y+1);	
	for(t=0;t<16;t++)
		BSP_OLED_WriteByte(Hzk[2*no+1][t],OLED_DATA);
}

/**********************************************************************
*	函 数 名: BSP_OLED_DrawBMP
*	功能说明: 显示BMP图片
*	形    参: [x0,y0],(x1,y1): 图片的左上角和右下角坐标，
*            x: 0~127，列号	 y: 0~7，页号
						BMP:要显示的图片
*	返 回 值: 无
***********************************************************************/
void BSP_OLED_DrawBMP(u8 x0, u8 y0,u8 x1, u8 y1,unsigned char BMP[])
{ 	
	unsigned int j=0;
	unsigned char x,y;  
	for(y=y0;y<y1;y++){
		BSP_OLED_SetPos(x0,y);
    for(x=x0;x<x1;x++)
	   	BSP_OLED_WriteByte(BMP[j++],OLED_DATA);	    	
	}
} 

u8 OLED_GRAM[128][8]={0x0};
/**********************************************************************
* 本函数需要MCU开缓冲区
* 功能说明：画点函数
* 参数：x: 横坐标，0~127
*       y: 纵坐标，0~63
**********************************************************************/
void BSP_OLED_DrawPoint(u8 x, u8 y, u8 t)
{
	u8 pos, bx, temp = 0;
	if(x>127||y>63) return;
	pos = 7 - y/8; // 页号
	bx = y%8;      // 页内偏移
	temp = 1 << (7-bx);
	if(t)
		OLED_GRAM[x][pos] |= temp;
	else
		OLED_GRAM[x][pos] &= ~temp;
}


void OLED_Refresh_Gram(void)
{
	u8 i, n;
	for(i=0; i<8; i++){
		BSP_OLED_WriteByte(0xB0, OLED_CMD);
		BSP_OLED_WriteByte(0x00, OLED_CMD);
		BSP_OLED_WriteByte(0x10, OLED_CMD);
		for(n=0; n<128; n++)
			BSP_OLED_WriteByte(OLED_GRAM[n][i],OLED_DATA);
	}
}

/*********************************************************
矩阵块水平滚动(SSD1315驱动)
p1 - 起始页号(0~7)，p2 - 结束页号(p1 ~ 7)
y1 - 起始列号(0~127)，y2 - 结束列号(y1~127)
spd - 滚动速度，帧数值越小速度越快，
      0:5帧，1:64帧，2:128帧，3:256帧，4:3帧，5:4帧，6:25帧，7:2帧
dir - 滚动方向，0-向右，1-向左
*********************************************************/
void BSP_OLED_ScrollH(u8 p1, u8 p2, u8 y1, u8 y2, u8 speed, u8 dir)
{
	BSP_OLED_WriteByte(0x2E,OLED_CMD);  //禁止滚动
	BSP_OLED_WriteByte(0x26 + dir, OLED_CMD);  //26-向右，27左向滚动
	BSP_OLED_WriteByte(0x00,OLED_CMD);  //填充
	BSP_OLED_WriteByte(p1,OLED_CMD);    //起始页地址
	BSP_OLED_WriteByte(speed,OLED_CMD); 	//滚动速度
	BSP_OLED_WriteByte(p2,OLED_CMD);   	//结束页地址
	BSP_OLED_WriteByte(y1,OLED_CMD);  	//起始列
	BSP_OLED_WriteByte(y2,OLED_CMD);  	//结束列
	BSP_OLED_WriteByte(0x2F,OLED_CMD);  //开启滚动
}

/*********************************************************
整屏垂直滚动，指定页水平滚动
p1 - 起始页号(0~7)，p2 - 结束页号(p1 ~ 7)
     (x1,y1)页范围内整行水平滚动

off - 垂直方向滚动的起始行号，
     =0时，第0~63行显示在第0~63行，实际垂直不滚动
     =1时，第1~63,0行显示在第0~63行，向上滚动
     ...
     =63时，第63,0~62行显示在第0~63行，向下滚动

spd - 水平滚动速度，值越小速度越快
dir - 水平滚动方向，0-向右，1-向左
*********************************************************/
void BSP_OLED_ScrollHV1(u8 p1, u8 p2, u8 off, u8 speed, u8 dir)
{
	BSP_OLED_WriteByte(0x2E,OLED_CMD);  //禁止滚动
	BSP_OLED_WriteByte(0x29 + dir ,OLED_CMD);  //29-向右，2a向左滚动
	BSP_OLED_WriteByte(0x00,OLED_CMD);  //填充
	BSP_OLED_WriteByte(p1,OLED_CMD);   	//起始页地址
	BSP_OLED_WriteByte(speed,OLED_CMD); 	//滚动速度
	BSP_OLED_WriteByte(p2,OLED_CMD);   	//结束页地址
	BSP_OLED_WriteByte(off,OLED_CMD);   //垂直偏移
	BSP_OLED_WriteByte(0x2F,OLED_CMD);  //开启滚动
}

/*********************************************************
指定行垂直滚动，指定页水平滚动
p1 - 水平滚动起始页(0~7)，p2 - 水平滚动结束页(p1 ~ 7)

r1 - 垂直滚动起始行，r2 - 垂直滚动结束行

off - 垂直方向滚动的起始行号，=0时，就只有水平滚动
      水平滚动以外的行垂直滚动

spd - 水平滚动速度，值越小速度越快
dir - 水平滚动方向，0-向右，1-向左
*********************************************************/
void BSP_OLED_ScrollHV2(u8 p1, u8 p2, u8 off, u8 spd, u8 dir,	u8 r1, u8 r2)
{
	BSP_OLED_WriteByte(0x2E,OLED_CMD);  //禁止滚动
	BSP_OLED_WriteByte(0xA3,OLED_CMD);  //设置垂直滚动行区域
	BSP_OLED_WriteByte(r1,OLED_CMD);  //起始行
	BSP_OLED_WriteByte(r2,OLED_CMD);	//结束行	
	BSP_OLED_WriteByte(0x29 + dir ,OLED_CMD);  //29-向右，2a向左滚动
	BSP_OLED_WriteByte(0x01,OLED_CMD);  //0x01 开启水平滚动 0x00 关闭
	BSP_OLED_WriteByte(p1,OLED_CMD);   	//起始页地址
	BSP_OLED_WriteByte(spd,OLED_CMD); 	//滚动速度
	BSP_OLED_WriteByte(p2,OLED_CMD);   	//结束页地址
	BSP_OLED_WriteByte(off,OLED_CMD);   //垂直偏移
	BSP_OLED_WriteByte(0x2F,OLED_CMD);  //开启滚动
}


/*********************************************************
指定行垂直滚动，指定区域水平滚动
p1 - 水平滚动起始页(0~7)，p2 - 水平滚动结束页(p1 ~ 7)
y1 - 水平滚动起始列(0~127), y2 - 水平滚动结束页(y1~y2)

r1 - 垂直滚动起始行，r2 - 垂直滚动结束行

off - 垂直方向滚动的起始行号，=0时，就只有水平滚动
      水平滚动以外的行垂直滚动

spd - 水平滚动速度，值越小速度越快
dir - 水平滚动方向，0-向右，1-向左
*********************************************************/
void BSP_OLED_ScrollHV3(u8 p1, u8 p2, u8 y1, u8 y2, u8 off, u8 spd, u8 dir,	u8 r1, u8 r2)
{
	BSP_OLED_WriteByte(0x2E,OLED_CMD);  //禁止滚动
	BSP_OLED_WriteByte(0xA3,OLED_CMD);  //设置垂直滚动行区域
	BSP_OLED_WriteByte(r1,OLED_CMD);  	//起始行
	BSP_OLED_WriteByte(r2,OLED_CMD);		//结束行	
	BSP_OLED_WriteByte(0x29 + dir ,OLED_CMD);  //29-向右，2a向左滚动
	BSP_OLED_WriteByte(0x01,OLED_CMD);  //0x01 开启水平滚动 0x00 关闭
	BSP_OLED_WriteByte(p1,OLED_CMD);   	//起始页地址
	BSP_OLED_WriteByte(spd,OLED_CMD); 	//滚动速度
	BSP_OLED_WriteByte(p2,OLED_CMD);   	//结束页地址
	BSP_OLED_WriteByte(off,OLED_CMD);   //垂直偏移
	BSP_OLED_WriteByte(y1,OLED_CMD);		//起始列 SSD1306不起作用
	BSP_OLED_WriteByte(y2,OLED_CMD);		//结束列
	BSP_OLED_WriteByte(0x2F,OLED_CMD);  //开启滚动
}

void BSP_OLED_ScrollStop(void){
	BSP_OLED_WriteByte(0x2E,OLED_CMD);  //禁止滚动
}

void BSP_OLED_ScrollStart(void){
	BSP_OLED_WriteByte(0x2F,OLED_CMD);  //开启滚动
}




#if 1
/***** 初始化SSD1306*****************************
*  打开OLED电荷泵
*  调整屏幕显示方
*  打开屏幕显
*  其余使用复位值
****************************************************/
static void BSP_OLED_SD1306_Init(void)
{
	//OLED复位
  OLED_RST_Set();
	BSP_Delay_ms(10);
	OLED_RST_Clr();
	BSP_Delay_ms(10);
	OLED_RST_Set(); 
					  
	BSP_OLED_WriteByte(0xAE,OLED_CMD);//关闭屏幕显示
	BSP_OLED_WriteByte(0xA1,OLED_CMD);//左右翻转(默认0xa0，不翻转)
	BSP_OLED_WriteByte(0xC8,OLED_CMD);//上下翻转(默认0xc0，不翻转)	
	BSP_OLED_WriteByte(0x8D,OLED_CMD);//设置电荷泵为开启(0x14-使能，默认0x10，关闭)
	BSP_OLED_WriteByte(0x14,OLED_CMD);
	BSP_OLED_WriteByte(0xAF,OLED_CMD); //打开屏幕显示
	
	BSP_OLED_Clear();
	BSP_OLED_SetPos(0,0); 	
}

#else
// 重新初始化所有寄存器
static void BSP_OLED_SD1306_Init(void)
{
	//OLED复位
  OLED_RST_Set();
	BSP_Delay_ms(10);
	OLED_RST_Clr();
	BSP_Delay_ms(10);
	OLED_RST_Set(); 
					  
  BSP_OLED_WriteByte(0xAE,OLED_CMD);//关闭屏幕显示
	BSP_OLED_WriteByte(0x00,OLED_CMD);//设置列地址低4位为0
	BSP_OLED_WriteByte(0x10,OLED_CMD);//设置列地址高4位为0
	
	BSP_OLED_WriteByte(0x40,OLED_CMD);//设置起始行为0(0x00~0x3F)
	
	BSP_OLED_WriteByte(0x81,OLED_CMD);//设置对比度为0xCF(值越大越亮)
	BSP_OLED_WriteByte(0xCF,OLED_CMD);
	
	BSP_OLED_WriteByte(0xA1,OLED_CMD);//设置左右反置(默认0xa0正常显示)
	BSP_OLED_WriteByte(0xC8,OLED_CMD);//设置上下反置(默认0xc0正常显示)	
	BSP_OLED_WriteByte(0xA6,OLED_CMD);//设置正常显示(0xa7反显)
	
	BSP_OLED_WriteByte(0xA8,OLED_CMD);//设置利用率为64(15~64)	
	BSP_OLED_WriteByte(0x3f,OLED_CMD);
	
	BSP_OLED_WriteByte(0xD3,OLED_CMD);//设置显示偏移量为0(0x00~0x3F)
	BSP_OLED_WriteByte(0x00,OLED_CMD);
	
	BSP_OLED_WriteByte(0xD5,OLED_CMD);//设置分频因子和晶振频率(100帧/s？)
	BSP_OLED_WriteByte(0x80,OLED_CMD);
	
	BSP_OLED_WriteByte(0xD9,OLED_CMD);//设置预充电周期
	BSP_OLED_WriteByte(0xF1,OLED_CMD);
	
	BSP_OLED_WriteByte(0xDA,OLED_CMD);//设置COM引脚硬件配置
	BSP_OLED_WriteByte(0x12,OLED_CMD);
	
	BSP_OLED_WriteByte(0xDB,OLED_CMD);//设置VCOMh电压
	BSP_OLED_WriteByte(0x20,OLED_CMD);
	
	BSP_OLED_WriteByte(0x20,OLED_CMD);//设置RAM寻址方式(0x000-水平/0x01-垂直/0x02-页)
	BSP_OLED_WriteByte(0x02,OLED_CMD);
	
	BSP_OLED_WriteByte(0x8D,OLED_CMD);//设置电荷泵为开启(0x14-使能，0x10-关闭)
	BSP_OLED_WriteByte(0x14,OLED_CMD);
	
	BSP_OLED_WriteByte(0xA4,OLED_CMD);//正常显示(0xa4/0xa5-全亮显示)
	BSP_OLED_WriteByte(0xA6,OLED_CMD);//正常显示(0xa6/a7-反显) 
	
	BSP_OLED_WriteByte(0xAF,OLED_CMD); //打开屏幕显示
	
	BSP_OLED_Clear();
	BSP_OLED_SetPos(0,0); 	
}
	
#endif

/*
***************************************************************************
*	函 数 名: BSP_OLED_Init
*	功能说明: 初始化SSD1306
*	形    参: 无
*	返 回 值: 无
***************************************************************************
*/
void BSP_OLED_Init(void)
{ 	
	BSP_OLED_GPIO_Init();
	BSP_OLED_SD1306_Init();
	BSP_OLED_All();	
}  

