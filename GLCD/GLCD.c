/****************************************Copyright (c)**************************************************                         
**
**                                 http://www.powermcu.com
**
**--------------File Info-------------------------------------------------------------------------------
** File name:			GLCD.c
** Descriptions:		Has been tested SSD1289、ILI9320、R61505U、SSD1298、ST7781、SPFD5408B、ILI9325、ILI9328、
**						HX8346A、HX8347A
**------------------------------------------------------------------------------------------------------
** Created by:			AVRman
** Created date:		2012-3-10
** Version:					1.3
** Descriptions:		The original version
**
**------------------------------------------------------------------------------------------------------
** Modified by:			Paolo Bernardi
** Modified date:		03/01/2020
** Version:					2.0
** Descriptions:		simple arrangement for screen usage
********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "GLCD.h" 
#include "AsciiLib.h"

/* Private variables ---------------------------------------------------------*/
static uint8_t LCD_Code;
static uint16_t back_color;

/* Private define ------------------------------------------------------------*/
#define  ILI9320    0  /* 0x9320 */
#define  ILI9325    1  /* 0x9325 */
#define  ILI9328    2  /* 0x9328 */
#define  ILI9331    3  /* 0x9331 */
#define  SSD1298    4  /* 0x8999 */
#define  SSD1289    5  /* 0x8989 */
#define  ST7781     6  /* 0x7783 */
#define  LGDP4531   7  /* 0x4531 */
#define  SPFD5408B  8  /* 0x5408 */
#define  R61505U    9  /* 0x1505 0x0505 */
#define  HX8346A		10 /* 0x0046 */  
#define  HX8347D    11 /* 0x0047 */
#define  HX8347A    12 /* 0x0047 */	
#define  LGDP4535   13 /* 0x4535 */  
#define  SSD2119    14 /* 3.5 LCD 0x9919 */

/*******************************************************************************
* Function Name  : Lcd_Configuration
* Description    : Configures LCD Control lines
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void LCD_Configuration(void)
{
	/* Configure the LCD Control pins */
	
	/* EN = P0.19 , LE = P0.20 , DIR = P0.21 , CS = P0.22 , RS = P0.23 , RS = P0.23 */
	/* RS = P0.23 , WR = P0.24 , RD = P0.25 , DB[0.7] = P2.0...P2.7 , DB[8.15]= P2.0...P2.7 */  
	LPC_GPIO0->FIODIR   |= 0x03f80000;
	LPC_GPIO0->FIOSET    = 0x03f80000;
}

/*******************************************************************************
* Function Name  : LCD_Send
* Description    : LCD写数据
* Input          : - byte: byte to be sent
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static __attribute__((always_inline)) void LCD_Send (uint16_t byte) 
{
	LPC_GPIO2->FIODIR |= 0xFF;          /* P2.0...P2.7 Output */
	LCD_DIR(1)		   				    				/* Interface A->B */
	LCD_EN(0)	                        	/* Enable 2A->2B */
	LPC_GPIO2->FIOPIN =  byte;          /* Write D0..D7 */
	LCD_LE(1)                         
	LCD_LE(0)														/* latch D0..D7	*/
	LPC_GPIO2->FIOPIN =  byte >> 8;     /* Write D8..D15 */
}

/*******************************************************************************
* Function Name  : wait_delay
* Description    : Delay Time
* Input          : - nCount: Delay Time
* Output         : None
* Return         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void wait_delay(int count)
{
	while(count--);
}

/*******************************************************************************
* Function Name  : LCD_Read
* Description    : LCD读数据
* Input          : - byte: byte to be read
* Output         : None
* Return         : 返回读取到的数据
* Attention		 : None
*******************************************************************************/
static __attribute__((always_inline)) uint16_t LCD_Read (void) 
{
	uint16_t value;
	
	LPC_GPIO2->FIODIR &= ~(0xFF);              /* P2.0...P2.7 Input */
	LCD_DIR(0);		   				           				 /* Interface B->A */
	LCD_EN(0);	                               /* Enable 2B->2A */
	wait_delay(30);							   						 /* delay some times */
	value = LPC_GPIO2->FIOPIN0;                /* Read D8..D15 */
	LCD_EN(1);	                               /* Enable 1B->1A */
	wait_delay(30);							   						 /* delay some times */
	value = (value << 8) | LPC_GPIO2->FIOPIN0; /* Read D0..D7 */
	LCD_DIR(1);
	return  value;
}

/*******************************************************************************
* Function Name  : LCD_WriteIndex
* Description    : LCD写寄存器地址
* Input          : - index: 寄存器地址
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static __attribute__((always_inline)) void LCD_WriteIndex(uint16_t index)
{
	LCD_CS(0);
	LCD_RS(0);
	LCD_RD(1);
	LCD_Send( index ); 
	wait_delay(22);	
	LCD_WR(0);  
	wait_delay(1);
	LCD_WR(1);
	LCD_CS(1);
}

/*******************************************************************************
* Function Name  : LCD_WriteData
* Description    : LCD写寄存器数据
* Input          : - index: 寄存器数据
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static __attribute__((always_inline)) void LCD_WriteData(uint16_t data)
{				
	LCD_CS(0);
	LCD_RS(1);   
	LCD_Send( data );
	LCD_WR(0);     
	wait_delay(1);
	LCD_WR(1);
	LCD_CS(1);
}

/*******************************************************************************
* Function Name  : LCD_ReadData
* Description    : 读取控制器数据
* Input          : None
* Output         : None
* Return         : 返回读取到的数据
* Attention		 : None
*******************************************************************************/
static __attribute__((always_inline)) uint16_t LCD_ReadData(void)
{ 
	uint16_t value;
	
	LCD_CS(0);
	LCD_RS(1);
	LCD_WR(1);
	LCD_RD(0);
	value = LCD_Read();
	
	LCD_RD(1);
	LCD_CS(1);
	
	return value;
}

/*******************************************************************************
* Function Name  : LCD_WriteReg
* Description    : Writes to the selected LCD register.
* Input          : - LCD_Reg: address of the selected register.
*                  - LCD_RegValue: value to write to the selected register.
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static __attribute__((always_inline)) void LCD_WriteReg(uint16_t LCD_Reg,uint16_t LCD_RegValue)
{ 
	/* Write 16-bit Index, then Write Reg */  
	LCD_WriteIndex(LCD_Reg);         
	/* Write 16-bit Reg */
	LCD_WriteData(LCD_RegValue);  
}

/*******************************************************************************
* Function Name  : LCD_WriteReg
* Description    : Reads the selected LCD Register.
* Input          : None
* Output         : None
* Return         : LCD Register Value.
* Attention		 : None
*******************************************************************************/
static __attribute__((always_inline)) uint16_t LCD_ReadReg(uint16_t LCD_Reg)
{
	uint16_t LCD_RAM;
	
	/* Write 16-bit Index (then Read Reg) */
	LCD_WriteIndex(LCD_Reg);
	/* Read 16-bit Reg */
	LCD_RAM = LCD_ReadData();      	
	return LCD_RAM;
}

/*******************************************************************************
* Function Name  : LCD_SetCursor
* Description    : Sets the cursor position.
* Input          : - Xpos: specifies the X position.
*                  - Ypos: specifies the Y position. 
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void LCD_SetCursor(uint16_t Xpos,uint16_t Ypos)
{
    #if  ( DISP_ORIENTATION == 90 ) || ( DISP_ORIENTATION == 270 )
	
 	uint16_t temp = Xpos;

			 Xpos = Ypos;
			 Ypos = ( MAX_X - 1 ) - temp;  

	#elif  ( DISP_ORIENTATION == 0 ) || ( DISP_ORIENTATION == 180 )
		
	#endif

  switch( LCD_Code )
  {
     default:		 /* 0x9320 0x9325 0x9328 0x9331 0x5408 0x1505 0x0505 0x7783 0x4531 0x4535 */
          LCD_WriteReg(0x0020, Xpos );     
          LCD_WriteReg(0x0021, Ypos );     
	      break; 

     case SSD1298: 	 /* 0x8999 */
     case SSD1289:   /* 0x8989 */
	      LCD_WriteReg(0x004e, Xpos );      
          LCD_WriteReg(0x004f, Ypos );          
	      break;  

     case HX8346A: 	 /* 0x0046 */
     case HX8347A: 	 /* 0x0047 */
     case HX8347D: 	 /* 0x0047 */
	      LCD_WriteReg(0x02, Xpos>>8 );                                                  
	      LCD_WriteReg(0x03, Xpos );  

	      LCD_WriteReg(0x06, Ypos>>8 );                           
	      LCD_WriteReg(0x07, Ypos );    
	
	      break;     
     case SSD2119:	 /* 3.5 LCD 0x9919 */
	      break; 
  }
}

/*******************************************************************************
* Function Name  : LCD_Delay
* Description    : Delay Time
* Input          : - nCount: Delay Time
* Output         : None
* Return         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void delay_ms(uint16_t ms)    
{ 
	uint16_t i,j; 
	for( i = 0; i < ms; i++ )
	{ 
		for( j = 0; j < 1141; j++ );
	}
} 


/*******************************************************************************
* Function Name  : LCD_Initializtion
* Description    : Initialize TFT Controller.
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void LCD_Initialization(void)
{
	uint16_t DeviceCode;
	
	LCD_Configuration();
	delay_ms(300);
	DeviceCode = LCD_ReadReg(0x0000);		/* 读取屏ID	*/	
	
	if( DeviceCode == 0x9325 || DeviceCode == 0x9328 )	
	{
		LCD_Code = ILI9325;
		LCD_WriteReg(0x00e7,0x0010);      
		LCD_WriteReg(0x0000,0x0001);  	/* start internal osc */
		LCD_WriteReg(0x0001,0x0100);     
		LCD_WriteReg(0x0002,0x0700); 	/* power on sequence */
		LCD_WriteReg(0x0003,0x1018); 	/* Entry Mode Set*/
		LCD_WriteReg(0x0004,0x0000);                                   
		LCD_WriteReg(0x0008,0x0207);	           
		LCD_WriteReg(0x0009,0x0000);         
		LCD_WriteReg(0x000a,0x0000); 	/* display setting */        
		LCD_WriteReg(0x000c,0x0001);	/* display setting */        
		LCD_WriteReg(0x000d,0x0000);
		LCD_WriteReg(0x000f,0x0000);
		/* Power On sequence */
		LCD_WriteReg(0x0010,0x0000);   
		LCD_WriteReg(0x0011,0x0007);
		LCD_WriteReg(0x0012,0x0000);                                                                 
		LCD_WriteReg(0x0013,0x0000);                 
		delay_ms(50);  /* delay 50 ms */		
		LCD_WriteReg(0x0010,0x1590);   
		LCD_WriteReg(0x0011,0x0227);
		delay_ms(50);  /* delay 50 ms */		
		LCD_WriteReg(0x0012,0x009c);                  
		delay_ms(50);  /* delay 50 ms */		
		LCD_WriteReg(0x0013,0x1900);   
		LCD_WriteReg(0x0029,0x0023);
		LCD_WriteReg(0x002b,0x000e);
		delay_ms(50);  /* delay 50 ms */		
		LCD_WriteReg(0x0020,0x0000);                                                            
		LCD_WriteReg(0x0021,0x0000);           
		delay_ms(50);  /* delay 50 ms */		
		LCD_WriteReg(0x0030,0x0007); 
		LCD_WriteReg(0x0031,0x0707);   
		LCD_WriteReg(0x0032,0x0006);
		LCD_WriteReg(0x0035,0x0704);
		LCD_WriteReg(0x0036,0x1f04); 
		LCD_WriteReg(0x0037,0x0004);
		LCD_WriteReg(0x0038,0x0000);        
		LCD_WriteReg(0x0039,0x0706);     
		LCD_WriteReg(0x003c,0x0701);
		LCD_WriteReg(0x003d,0x000f);
		delay_ms(50);  /* delay 50 ms */		
		LCD_WriteReg(0x0050,0x0000);        
		LCD_WriteReg(0x0051,0x00ef);   
		LCD_WriteReg(0x0052,0x0000);     
		LCD_WriteReg(0x0053,0x013f);
		LCD_WriteReg(0x0060,0xa700);        
		LCD_WriteReg(0x0061,0x0001); 
		LCD_WriteReg(0x006a,0x0000);
		LCD_WriteReg(0x0080,0x0000);
		LCD_WriteReg(0x0081,0x0000);
		LCD_WriteReg(0x0082,0x0000);
		LCD_WriteReg(0x0083,0x0000);
		LCD_WriteReg(0x0084,0x0000);
		LCD_WriteReg(0x0085,0x0000);
		  
		LCD_WriteReg(0x0090,0x0010);     
		LCD_WriteReg(0x0092,0x0000);  
		LCD_WriteReg(0x0093,0x0003);
		LCD_WriteReg(0x0095,0x0110);
		LCD_WriteReg(0x0097,0x0000);        
		LCD_WriteReg(0x0098,0x0000);  
		/* display on sequence */    
		LCD_WriteReg(0x0007,0x0133);
		
		LCD_WriteReg(0x0020,0x0000);  /* 行首址0 */                                                          
		LCD_WriteReg(0x0021,0x0000);  /* 列首址0 */     
	}	else if( DeviceCode == 0x9320 || DeviceCode == 0x9300 )  {
    LCD_Code = ILI9320;
    LCD_WriteReg(0x00,0x0000);	/* Start Oscillation (don't start here) */
    LCD_WriteReg(0x01,0x0100);	/* Driver Output Control */
    LCD_WriteReg(0x02,0x0700);	/* LCD Driver Waveform Control */
    LCD_WriteReg(0x03,0x1018);	/* Entry Mode Set (if HVM = 1 : High speed write function enabled)*/

    LCD_WriteReg(0x04,0x0000);	/* Resizing Control Register */
    LCD_WriteReg(0x08,0x0202);	/* Display Control 2 (Porch) */
    LCD_WriteReg(0x09,0x0000);	/* Display Contral 3.(0x0000) */
    LCD_WriteReg(0x0a,0x0000);	/* Frame Cycle Contal.(0x0000) */
		LCD_WriteReg(0x0b,0x0001);	/* RGB Display Interface Control 1 */
    LCD_WriteReg(0x0c,(1<<0));	/* Extern Display Interface Contral */
    LCD_WriteReg(0x0d,0x0000);	/* Frame Maker Position */
    LCD_WriteReg(0x0f,0x0000);	/* Extern Display Interface Contral 2. */

    delay_ms(100);  /* delay 100 ms */
    LCD_WriteReg(0x07,0x0101);	/* Display Contral */
    delay_ms(100);  /* delay 100 ms */

    LCD_WriteReg(0x10,(1<<12)|(0<<8)|(1<<7)|(1<<6)|(0<<4));	/* Power Control 1.(0x16b0)	*/
    LCD_WriteReg(0x11,0x0007);								/* Power Control 2 */
    LCD_WriteReg(0x12,(1<<8)|(1<<4)|(0<<0));				/* Power Control 3.(0x0138)	*/
    LCD_WriteReg(0x13,0x0b00);								/* Power Control 4 */
    LCD_WriteReg(0x29,0x0000);								/* Power Control 7 */

    LCD_WriteReg(0x2b,(1<<14)|(1<<4));

		/* Window Address Area */
    LCD_WriteReg(0x50,0);       /* Set X Start */
    LCD_WriteReg(0x51,239);	    /* Set X End */
    LCD_WriteReg(0x52,0);	      /* Set Y Start */
    LCD_WriteReg(0x53,319);	    /* Set Y End */

    LCD_WriteReg(0x60,0x2700);	/* Driver Output Control */
    LCD_WriteReg(0x61,0x0001);	/* Driver Output Control */
    LCD_WriteReg(0x6a,0x0000);	/* Vertical Srcoll Control */

    LCD_WriteReg(0x80,0x0000);	/* Display Position? Partial Display 1 */
    LCD_WriteReg(0x81,0x0000);	/* RAM Address Start? Partial Display 1 */
    LCD_WriteReg(0x82,0x0000);	/* RAM Address End-Partial Display 1 */
    LCD_WriteReg(0x83,0x0000);	/* Displsy Position? Partial Display 2 */
    LCD_WriteReg(0x84,0x0000);	/* RAM Address Start? Partial Display 2 */
    LCD_WriteReg(0x85,0x0000);	/* RAM Address End? Partial Display 2 */

    LCD_WriteReg(0x90,(0<<7)|(16<<0));	/* Frame Cycle Contral.(0x0013)	*/
    LCD_WriteReg(0x92,0x0000);	/* Panel Interface Contral 2.(0x0000) */
    LCD_WriteReg(0x93,0x0001);	/* Panel Interface Contral 3. */
    LCD_WriteReg(0x95,0x0110);	/* Frame Cycle Contral.(0x0110)	*/
    LCD_WriteReg(0x97,(0<<8));
    LCD_WriteReg(0x98,0x0000);	/* Frame Cycle Contral */

    LCD_WriteReg(0x07,0x0173);
  }

    delay_ms(50);   /* delay 50 ms */	
		LCD_SetBackground(White);
}

/*******************************************************************************
* Function Name  : LCD_Clear
* Description    : 将屏幕填充成指定的颜色，如清屏，则填充 0xffff
* Input          : - Color: Screen Color
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void LCD_Clear(uint16_t Color)
{
	uint32_t index;
	
	if( LCD_Code == HX8347D || LCD_Code == HX8347A )
	{
		LCD_WriteReg(0x02,0x00);                                                  
		LCD_WriteReg(0x03,0x00);  
		                
		LCD_WriteReg(0x04,0x00);                           
		LCD_WriteReg(0x05,0xEF);  
		                 
		LCD_WriteReg(0x06,0x00);                           
		LCD_WriteReg(0x07,0x00);    
		               
		LCD_WriteReg(0x08,0x01);                           
		LCD_WriteReg(0x09,0x3F);     
	}
	else
	{	
		LCD_SetCursor(0,0); 
	}	
/*
	LCD_WriteIndex(0x0022);
	for( index = 0; index < MAX_X * MAX_Y; index++ )
	{
		LCD_WriteData(Color);
	}
*/	
	LCD_ClearWindow();
	LCD_CS(0);
	LCD_WriteCommand(0x22);
	LCD_RS(1);
	for (index = 0; index < MAX_X*MAX_Y; index++) {
		LCD_WritePixel(Color);
	}
	LCD_CS(1);
}

/******************************************************************************
* Function Name  : LCD_BGR2RGB
* Description    : RRRRRGGGGGGBBBBB 改为 BBBBBGGGGGGRRRRR 格式
* Input          : - color: BRG 颜色值  
* Output         : None
* Return         : RGB 颜色值
* Attention		 : 内部函数调用
*******************************************************************************/
static uint16_t LCD_BGR2RGB(uint16_t color)
{
	uint16_t  r, g, b, rgb;
	
	b = ( color>>0 )  & 0x1f;
	g = ( color>>5 )  & 0x3f;
	r = ( color>>11 ) & 0x1f;
	
	rgb =  (b<<11) + (g<<5) + (r<<0);
	
	return( rgb );
}

/******************************************************************************
* Function Name  : LCD_GetPoint
* Description    : 获取指定座标的颜色值
* Input          : - Xpos: Row Coordinate
*                  - Xpos: Line Coordinate 
* Output         : None
* Return         : Screen Color
* Attention		 : None
*******************************************************************************/
uint16_t LCD_GetPoint(uint16_t Xpos,uint16_t Ypos)
{
	uint16_t dummy;
	
	LCD_SetCursor(Xpos,Ypos);
	LCD_WriteIndex(0x0022);  
	
	switch( LCD_Code )
	{
		case ST7781:
		case LGDP4531:
		case LGDP4535:
		case SSD1289:
		case SSD1298:
             dummy = LCD_ReadData();   /* Empty read */
             dummy = LCD_ReadData(); 	
 		     return  dummy;	      
	    case HX8347A:
	    case HX8347D:
             {
		        uint8_t red,green,blue;
				
				dummy = LCD_ReadData();   /* Empty read */

		        red = LCD_ReadData() >> 3; 
                green = LCD_ReadData() >> 2; 
                blue = LCD_ReadData() >> 3; 
                dummy = (uint16_t) ( ( red<<11 ) | ( green << 5 ) | blue ); 
		     }	
	         return  dummy;

        default:	/* 0x9320 0x9325 0x9328 0x9331 0x5408 0x1505 0x0505 0x9919 */
             dummy = LCD_ReadData();   /* Empty read */
             dummy = LCD_ReadData(); 	
 		     return  LCD_BGR2RGB( dummy );
	}
}

/******************************************************************************
* Function Name  : LCD_SetPoint
* Description    : 在指定座标画点
* Input          : - Xpos: Row Coordinate
*                  - Ypos: Line Coordinate 
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void LCD_SetPoint(uint16_t Xpos,uint16_t Ypos,uint16_t point)
{
	if( Xpos >= MAX_X || Ypos >= MAX_Y )
	{
		return;
	}
	LCD_SetCursor(Xpos,Ypos);
	LCD_WriteReg(0x0022,point);
}

/******************************************************************************
* Function Name  : LCD_DrawLine
* Description    : Bresenham's line algorithm
* Input          : - x1: A点行座标
*                  - y1: A点列座标 
*				   - x2: B点行座标
*				   - y2: B点列座标 
*				   - color: 线颜色
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/	 
void LCD_DrawLine( uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1 , uint16_t color )
{
    short dx,dy;      /* 定义X Y轴上增加的变量值 */
    short temp;       /* 起点 终点大小比较 交换数据时的中间变量 */

    if( x0 > x1 )     /* X轴上起点大于终点 交换数据 */
    {
	    temp = x1;
		x1 = x0;
		x0 = temp;   
    }
    if( y0 > y1 )     /* Y轴上起点大于终点 交换数据 */
    {
		temp = y1;
		y1 = y0;
		y0 = temp;   
    }
  
	dx = x1-x0;       /* X轴方向上的增量 */
	dy = y1-y0;       /* Y轴方向上的增量 */

    if( dx == 0 )     /* X轴上没有增量 画垂直线 */ 
    {
        do
        { 
            LCD_SetPoint(x0, y0, color);   /* 逐点显示 描垂直线 */
            y0++;
        }
        while( y1 >= y0 ); 
		return; 
    }
    if( dy == 0 )     /* Y轴上没有增量 画水平直线 */ 
    {
        do
        {
            LCD_SetPoint(x0, y0, color);   /* 逐点显示 描水平线 */
            x0++;
        }
        while( x1 >= x0 ); 
		return;
    }
	/* 布兰森汉姆(Bresenham)算法画线 */
    if( dx > dy )                         /* 靠近X轴 */
    {
	    temp = 2 * dy - dx;               /* 计算下个点的位置 */         
        while( x0 != x1 )
        {
	        LCD_SetPoint(x0,y0,color);    /* 画起点 */ 
	        x0++;                         /* X轴上加1 */
	        if( temp > 0 )                /* 判断下下个点的位置 */
	        {
	            y0++;                     /* 为右上相邻点，即（x0+1,y0+1） */ 
	            temp += 2 * dy - 2 * dx; 
	 	    }
            else         
            {
			    temp += 2 * dy;           /* 判断下下个点的位置 */  
			}       
        }
        LCD_SetPoint(x0,y0,color);
    }  
    else
    {
	    temp = 2 * dx - dy;                      /* 靠近Y轴 */       
        while( y0 != y1 )
        {
	 	    LCD_SetPoint(x0,y0,color);     
            y0++;                 
            if( temp > 0 )           
            {
                x0++;               
                temp+=2*dy-2*dx; 
            }
            else
			{
                temp += 2 * dy;
			}
        } 
        LCD_SetPoint(x0,y0,color);
	}
} 




void LCD_DrawLine2( uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1 , uint16_t color )
{
  short dx,dy;      /* 定义X Y轴上增加的变量值c */
  short temp;       /* 起点 终点大小比较 交换数据时的中间变量a */
  
  short ydir=1, xdir=1;

  if( x0 > x1 )     
  {
    xdir = -1;
  }
  if( y0 > y1 )     
  {
    ydir = -1;
  }

  dx = x1-x0;       /* X轴方向上的增量r */
  dy = y1-y0;       /* Y轴方向上的增量b */

  if( dx == 0 )     /* X轴上没有增量 画垂直线o */ 
  {
    do
    { 
      LCD_SetPoint(x0, y0, color);   /* 逐点显示 描垂直线n */
      y0 += ydir;
    }
    while( y0 != y1 );
    LCD_SetPoint(x0, y0, color);
    return; 
  }
  if( dy == 0 )     /* Y轴上没有增量 画水平直线e */ 
  {
    do
    {
      LCD_SetPoint(x0, y0, color);   /* 逐点显示 描水平线f */
      x0 += xdir;
    }
    while( x0 != x1 );
    LCD_SetPoint(x0, y0, color);
    return;
  }
/* 布兰森汉姆(Bresenham)算法画线e */
  if( dx > dy )                         /* 靠近X轴d */
  {
    temp = 2 * dy - dx;               /* 计算下个点的位置e */         
    while( x0 != x1 )
    {
      LCD_SetPoint(x0,y0,color);    /* 画起点r */ 
      x0 += xdir;                         /* X轴上加1i */
      if( temp != 0 )                /* 判断下下个点的位置c */
      {
        y0 += ydir;                     /* 为右上相邻点，即（x0+1,y0+1）o */ 
        temp += 2 * dy - 2 * dx; 
      }
      else         
      {
        temp += 2 * dy;           /* 判断下下个点的位置 */  
      }       
    }
    LCD_SetPoint(x0,y0,color);
  }  
  else
  {
    temp = 2 * dx - dy;                      /* 靠近Y轴 */       
    while( y0 != y1 )
    {
      LCD_SetPoint(x0,y0,color);     
      y0 += ydir;                 
      if( temp != 0 )           
      {
        x0 += xdir;               
        temp+=2*dy-2*dx; 
      }
      else
      {
        temp += 2 * dy;
      }
    } 
  LCD_SetPoint(x0,y0,color);
  }
}


/******************************************************************************
* Function Name  : PutChar
* Description    : 将Lcd屏上任意位置显示一个字符
* Input          : - Xpos: 水平坐标 
*                  - Ypos: 垂直坐标  
*				   - ASCI: 显示的字符
*				   - charColor: 字符颜色   
*				   - bkColor: 背景颜色 
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void PutChar( uint16_t Xpos, uint16_t Ypos, uint8_t ASCI, uint16_t charColor, uint16_t bkColor )
{
	uint16_t i, j;
    uint8_t buffer[16], tmp_char;
    GetASCIICode(buffer,ASCI);  /* 取字模数据 */
    for( i=0; i<16; i++ )
    {
        tmp_char = buffer[i];
        for( j=0; j<8; j++ )
        {
            if( ((tmp_char >> (7 - j)) & 0x01) == 0x01 )
            {
                LCD_SetPoint( Xpos + j, Ypos + i, charColor );  /* 字符颜色 */
            }
            else
            {
                LCD_SetPoint( Xpos + j, Ypos + i, bkColor );  /* 背景颜色 */
            }
        }
    }
}

void PutCharHorizontal( uint16_t Xpos, uint16_t Ypos, uint8_t ASCI, uint16_t charColor, uint16_t bkColor )
{
	uint16_t i, j;
    uint8_t buffer[16], tmp_char;
    GetASCIICode(buffer,ASCI);  /* 取字模数据 */
    for( i=0; i<16; i++ )
    {
        tmp_char = buffer[i];
        for( j=0; j<8; j++ )
        {
            if( ((tmp_char >> (7 - j)) & 0x01) == 0x01 )
            {
                LCD_SetPoint( Ypos + i, Xpos + j, charColor );  /* 字符颜色 */
            }
            else
            {
                LCD_SetPoint( Ypos + i, Xpos + j, bkColor );  /* 背景颜色 */
            }
        }
    }
}

/******************************************************************************
* Function Name  : GUI_Text
* Description    : 在指定座标显示字符串
* Input          : - Xpos: 行座标
*                  - Ypos: 列座标 
*				   - str: 字符串
*				   - charColor: 字符颜色   
*				   - bkColor: 背景颜色 
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void GUI_Text(uint16_t Xpos, uint16_t Ypos, uint8_t *str,uint16_t Color, uint16_t bkColor)
{
    uint8_t TempChar;
    do
    {
        TempChar = *str++;  
        PutChar( Xpos, Ypos, TempChar, Color, bkColor );    
        if( Xpos < MAX_X - 8 )
        {
            Xpos += 8;
        } 
        else if ( Ypos < MAX_Y - 16 )
        {
            Xpos = 0;
            Ypos += 16;
        }   
        else
        {
            Xpos = 0;
            Ypos = 0;
        }    
    }
    while ( *str != 0 );
}

void wait()
{
	__ASM("nop");
}

void LCD_WriteCommand (uint8_t index)
{
	LCD_RS(0)
  LCD_RD(1)
  LCD_Send(index);
  LCD_WR(0)
  wait();
  LCD_WR(1)
}

void LCD_WritePixel (uint16_t pixel)
{
	LCD_Send(pixel);
  LCD_WR(0)
  wait();
  LCD_WR(1)
}

void LCD_ClearWindow (void)
{
	/* Window Address Area */
	LCD_WriteReg(0x50,0);       									/* Set X Start 	*/
	LCD_WriteReg(0x51,239);	    									/* Set X End 		*/
	LCD_WriteReg(0x52,0);	   									  	/* Set Y Start 	*/
	LCD_WriteReg(0x53,319);										    /* Set Y End 		*/
}

void LCD_SetWindow (uint16_t X_start, uint16_t Y_start, uint16_t width, uint16_t height)
{
	/* Window Address Area	*/
	LCD_WriteReg(0x50,X_start);       						/* Set X Start	*/
	LCD_WriteReg(0x51,X_start + width - 1);				/* Set X End 		*/
	LCD_WriteReg(0x52,Y_start);	      						/* Set Y Start 	*/
	LCD_WriteReg(0x53,Y_start + height -1);				/* Set Y End 		*/
	/* Set pos */
	LCD_WriteReg(0x20, X_start);
	LCD_WriteReg(0x21, Y_start);
}

void LCD_SetOrientation(uint8_t orientation)
{
	switch (orientation)
  {
  	case 'V':
			LCD_WriteReg(0x03,0x1030);
  		break;
  	case 'H':
			LCD_WriteReg(0x03,0x1018);
  		break;
  	default:
  		break;
  }
	return;
}

void pixel(uint16_t xc,uint16_t yc,uint16_t x,uint16_t y, uint16_t color)
{
	LCD_SetPoint(xc+x,yc+y,color);
	LCD_SetPoint(xc+x,yc-y,color);
	LCD_SetPoint(xc-x,yc+y,color);
	LCD_SetPoint(xc-x,yc-y,color);
	LCD_SetPoint(xc+y,yc+x,color);
	LCD_SetPoint(xc+y,yc-x,color);
	LCD_SetPoint(xc-y,yc+x,color);
	LCD_SetPoint(xc-y,yc-x,color);
}

void LCD_DrawCircle(uint16_t Xpos,uint16_t Ypos, uint16_t radius, uint16_t color)
{
	int x=0;
	int y=radius;
	int p=1-radius;
	pixel(Xpos,Ypos,x,y, color);
	
	while(x<y)
	{
		if(p<0)
		{
			x++;
			p=p+2*x+1;
		}
		else
		{
			x++;
			y--;
			p=p+2*(x-y)+1;
		}
		pixel(Xpos,Ypos,x,y, color);
	}
}

void LCD_DrawRectangle (uint16_t X_start, uint16_t Y_start, uint16_t width, uint16_t height, uint16_t color)
{
	uint16_t i, j;
	
	for (i = 0; i < width; i++) {
		for (j = 0; j < height; j++) {
			LCD_SetPoint(X_start + i, Y_start + j, color);
		}
	}
	
}

void LCD_Darken(uint16_t X_start, uint16_t Y_start, uint16_t width, uint16_t height, uint16_t darken_factor)
{
	uint16_t i, j, pixel;
	uint8_t r, g, b;
		
	for (i = X_start; i < X_start + width; i++) {
		for (j = Y_start; j < Y_start + height; j++) {
			LCD_SetCursor(i, j);
			LCD_WriteIndex(0x22);
			pixel = LCD_ReadData();
			r = (pixel & 0xF800) >> 10;
			//r = r >= darken_factor ? r - darken_factor : 0;
			g = (pixel & 0x07E0) >> 5;
			//g = g >= darken_factor ? g - darken_factor : 0;
			b = (pixel & 0x001F);
			//b = b >= darken_factor ? b - darken_factor : 0;
			pixel = RGB565CONVERT(r, g, b);
			LCD_SetPoint(i, j, pixel);
		}
	}
}

uint16_t LCD_GetPixel(uint16_t i, uint16_t j)
{
	LCD_SetCursor(i, j);
	LCD_WriteIndex(0x22);
	return LCD_ReadData();
}

void LCD_SetBackground(uint16_t color)
{
	back_color = color;
	LCD_Clear(back_color);
}

uint16_t LCD_GetBackground(void)
{
	return back_color;
}


/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
