/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_timer.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    timer.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include <string.h>
#include "lpc17xx.h"
#include "timer.h"
#include "../GLCD/GLCD.h" 
#include "../TouchPanel/TouchPanel.h"
#include "../MyLib/Sprites.h"
#include "../Character.h"
#include "../reset.h"
#include "../init.h"



/******************************************************************************
** Function name:		Timer0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
volatile int down=0;
volatile int selright=0;
volatile int selleft=0;
volatile static int countLife = 4;
volatile static int recharge1=0;
static void snackF(){
	disable_timer(2);
	disable_timer(1);
	drawing(&snack,190,160);
	dyno.state++; 
	dyno.happiness=dyno.happiness+2;
	recharge1=1;
	
}
static void mealF(){
	disable_timer(2);
	disable_timer(1);
	drawing(&Meal,20,160);
	dyno.state=2; 
	dyno.satiety=dyno.satiety+2;
	recharge1=1;
	
}


void TIMER0_IRQHandler (void)  //TIMER GESTIONE ANIMAZIONI CHARACTER
{
	
	volatile static int count = 0;
	
	//Animation 
	count++;
	switch(dyno.state)
	{
		case 0:  //IDLE
			switch(count){
				case 1:
					drawing(&frame_0,100,140);
				break;
				case 2:
					drawing(&frame_1,100,140);
				break;
				case 3:
					drawing(&frame_2,100,140);
					count=0;
				break;
				}
			break;
			case 1: //WALK RIGHT
				switch(count){
				case 1:
					drawing(&frame_7,100,140);  
				break;
				case 2:
					drawing(&frame_8,100,140);  
				break;
				case 3:
					drawing(&whiteframe,100,140); 
					drawing(&frame_9,148,140);   
				break;
				case 4:
					  drawing(&whiteframe,148,140);
						drawing(&frame_10,172,140);
						drawing(&whiteframe,172,140);
						drawing(&frame_11,172,160); 
						drawing(&whiteframe,172,160);
						dyno.state--;
				    enable_timer(2);
						enable_timer(1);
						count=0;
					break;
				}
			break;
				case 2:  //WALK LEFT
					{
						switch(count){
							case 1:
								drawing(&frame_7Mirr,100,140);  
							break;
							case 2:
								drawing(&frame_8Mirr,100,140);  
							break;
							case 3:
								drawing(&whiteframe,100,140); 
								drawing(&frame_9Mirr,52,140);   
							break;
							case 4:
									drawing(&whiteframe,52,140);
									drawing(&frame_10Mirr,20,140);
									drawing(&whiteframe,20,140);
									drawing(&frame_11Mirr,20,160); 
									drawing(&whiteframe,20,160);
									dyno.state=0;
									enable_timer(2);
									enable_timer(1);
									count=0;
								break;
					}
				}
				break;
				case 3:  //RUN AWAY
				{
					switch(count){
							case 1:
								drawing(&frame_12,100,140);  
							break;
							case 2:
								drawing(&frame_17,100,140);  
							break;
							case 3: 
								drawing(&frame_18,100,140);   
							break;
							case 4:
									drawing(&whiteframe,100,140);
									drawing(&frame_19,124,140);
							break;
							case 5:
									drawing(&whiteframe,124,140);
									drawing(&frame_20,148,140); 
							break;
							case 6:
									drawing(&whiteframe,148,140);
									drawing(&frame_21,172,140); 
							break;
							case 7:
									drawing(&whiteframe,172,140);
									drawing(&frame_22,196,140); 
							break;
							case 8:
									drawing(&whiteframe,196,140);
									drawing(&frame_23,240,140); 
									drawing(&whiteframe,240,140);		
									count=0;
									ResetGame();
									disable_timer(0);
									
								break;
					}
					
				}
				break;
					
			default:
			break;
		
	}
	
	
	
	
	
	
  LPC_TIM0->IR = 1;			/* clear interrupt flag */
  return;
}


/******************************************************************************
** Function name:		Timer1_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void TIMER1_IRQHandler (void)   //TIMER GESTIONE JOYSTICK
{
	volatile static int left=0;
	volatile static int right=0;
	volatile static int select=0;
	

	
	if((LPC_GPIO1->FIOPIN & (1<<27)) == 0){	
		/* Joytick LEFT pressed */
		left++;
		switch(left){
			case 1:
				//SELEZIONE
				LCD_DrawLine(5, 265, 120, 265, Red);//linea orizzontale up
				LCD_DrawLine(5, 315, 120, 315, Red);//linea orizzontale down
				LCD_DrawLine(120, 265, 120, 315, Red); // linea verticale dx
				LCD_DrawLine(5, 265, 5, 315, Red); // linea verticale sx
				//RESET
				LCD_DrawLine(120, 265, 235, 265, Black);//linea orizzontale up
				LCD_DrawLine(120, 315, 235, 315, Black);//linea orizzontale down
				LCD_DrawLine(235, 265, 235, 315, Black); // linea verticale dx
				selleft=1;
				selright=0;
				break;
			default:
				break;
		}
	}
	else{
			left=0;
	}
	
	if((LPC_GPIO1->FIOPIN & (1<<28)) == 0){	
		/* Joytick RIGHT pressed */
		right++;
		switch(right){
			case 1:
				//RESET
				LCD_DrawLine(5, 265, 120, 265, Black);//linea orizzontale up
				LCD_DrawLine(5, 315, 120, 315, Black);//linea orizzontale down		
				LCD_DrawLine(5, 265, 5, 315, Black); // linea verticale sx
				//SELEZIONE
				LCD_DrawLine(120, 265, 235, 265, Red);//linea orizzontale up
				LCD_DrawLine(120, 315, 235, 315, Red);//linea orizzontale down
				LCD_DrawLine(120, 265, 120, 315, Red); // linea verticale dx
				LCD_DrawLine(235, 265, 235, 315, Red); // linea verticale sx
				selright=1;
				selleft=0;
				break;
			default:
				break;
		}
	}
	else{
			right=0;
	}
	
	/*Joystick select pressed*/
	if((LPC_GPIO1->FIOPIN & (1<<25)) == 0){
		select++;	
		switch(select){
			case 1:
				if(dyno.state==3)
				{
					selleft=0;
					selright=0;
					dyno.age=-1;
					dyno.state=0;
					dyno.satiety=4;
					dyno.happiness=4;
					countLife=4;
					InitGame();
				}
				if(selright==1)
				{
					snackF();		
				}
				if(selleft==1)              
				{
					
					mealF();				
				}
				break;
			default:
				break;
		}		
	}
	else{
		select=0;
	}
  LPC_TIM1->IR = 1;			/* clear interrupt flag */
  return;
}

void TIMER2_IRQHandler (void)  //TIMER GESTIONE HAPPINES,SATIETY,AGE
{
	volatile static int seconds=0;
	volatile static int minutes=0;
	volatile static int hours=0;
	char Strseconds[5]="";
	char Strminutes[2]="";
	char Strhours[2]="";
	char time_in_char2[5] = "";
	//Age
	dyno.age++;
	//age++;
	seconds=dyno.age%60;
	minutes=(dyno.age/60)%60;
	hours=(dyno.age/3600)%24;
	//formattazione stringhe
	if(seconds>=10){
		sprintf(Strseconds,"%d",seconds);
	}else{
		sprintf(Strseconds,"0%d",seconds);
	}
	if (minutes>=10){
	sprintf(Strminutes,"%d",minutes);
	}else{
		sprintf(Strminutes,"0%d",minutes);
	}
	if(hours>=10){
	sprintf(Strhours,"%d",hours);
	}else{
		sprintf(Strhours,"0%d",hours);
	}
	
	sprintf(time_in_char2,"Age: %s:%s:%s ",Strhours,Strminutes,Strseconds);
	GUI_Text(65, 10, (uint8_t *) time_in_char2, Black, White);
	//Health
	
	if (countLife<5)
	{
	countLife++;
	}
	if (countLife>5)
	{
		countLife--;
		recharge1=1;
	}
	if(dyno.happiness<0 || dyno.satiety<0)
	{
		disable_timer(1);
		LCD_DrawLine(235, 315, 235, 5, White);
		dyno.state=3;
		disable_timer(2);		
	}
	if(dyno.happiness>4)
	{
		dyno.happiness=4;
	}
	if(dyno.satiety>4)
	{
		dyno.satiety=4;
	}
	if(recharge1==0)
	{
			if(countLife==5)
		{
			switch(dyno.happiness)
			{
				case 4:
					//tacche batteria inziali happiness
					LCD_DrawRectangle(40,55,8,20,Red);
					LCD_DrawRectangle(50,55,8,20,Orange);
					LCD_DrawRectangle(60,55,8,20,DarkYellow);
					LCD_DrawRectangle(70,55,8,20,Green);
					dyno.happiness--;
					countLife=0;
					break;
					case 3:
						//tacche batteria inziali happiness
						LCD_DrawRectangle(40,55,8,20,Red);
						LCD_DrawRectangle(50,55,8,20,Orange);
						LCD_DrawRectangle(60,55,8,20,DarkYellow);
						LCD_DrawRectangle(70,55,8,20,White);
						dyno.happiness--;
						countLife=0;
						break;
						case 2:
							//tacche batteria inziali happiness
							LCD_DrawRectangle(40,55,8,20,Red);
							LCD_DrawRectangle(50,55,8,20,Orange);
							LCD_DrawRectangle(60,55,8,20,White);
							LCD_DrawRectangle(70,55,8,20,White);
							dyno.happiness--;
							countLife=0;
							break;
							case 1:
								//tacche batteria inziali happiness
								LCD_DrawRectangle(40,55,8,20,Red);
								LCD_DrawRectangle(50,55,8,20,White);
								LCD_DrawRectangle(60,55,8,20,White);
								LCD_DrawRectangle(70,55,8,20,White);
								dyno.happiness--;
								countLife=0;
								break;
								case 0:
									//tacche batteria inziali happiness
									LCD_DrawRectangle(40,55,8,20,White);
									LCD_DrawRectangle(50,55,8,20,White);
									LCD_DrawRectangle(60,55,8,20,White);
									LCD_DrawRectangle(70,55,8,20,White);
									dyno.happiness--;
									countLife=0;
								break;
			  }
				switch(dyno.satiety)
				{
				case 4:
					//tacche batteria iniziale satiety
					LCD_DrawRectangle(160,55,8,20,Red);
					LCD_DrawRectangle(170,55,8,20,Orange);
					LCD_DrawRectangle(180,55,8,20,DarkYellow);
					LCD_DrawRectangle(190,55,8,20,Green);
					dyno.satiety--;
					countLife=0;
					break;
					case 3:
						//tacche batteria iniziale satiety
						LCD_DrawRectangle(160,55,8,20,Red);
						LCD_DrawRectangle(170,55,8,20,Orange);
						LCD_DrawRectangle(180,55,8,20,DarkYellow);
						LCD_DrawRectangle(190,55,8,20,White);
						dyno.satiety--;
						countLife=0;
						break;
						case 2:
							//tacche batteria iniziale satiety
							LCD_DrawRectangle(160,55,8,20,Red);
							LCD_DrawRectangle(170,55,8,20,Orange);
							LCD_DrawRectangle(180,55,8,20,White);
							LCD_DrawRectangle(190,55,8,20,White);
							dyno.satiety--;
							countLife=0;
							break;
							case 1:
								//tacche batteria iniziale satiety
								LCD_DrawRectangle(160,55,8,20,Red);
								LCD_DrawRectangle(170,55,8,20,White);
								LCD_DrawRectangle(180,55,8,20,White);
								LCD_DrawRectangle(190,55,8,20,White);
								dyno.satiety--;
								countLife=0;
								break;
								case 0:
									//tacche batteria iniziale satiety
									LCD_DrawRectangle(160,55,8,20,White);
									LCD_DrawRectangle(170,55,8,20,White);
									LCD_DrawRectangle(180,55,8,20,White);
									LCD_DrawRectangle(190,55,8,20,White);
									dyno.satiety--;
									countLife=0;
								break;
			}
		}
	}
	else if(recharge1==1 && selleft==1)
	{
		switch(dyno.satiety)
			{
			case 4:
				//tacche batteria iniziale satiety
				LCD_DrawRectangle(160,55,8,20,Red);
				LCD_DrawRectangle(170,55,8,20,Orange);
				LCD_DrawRectangle(180,55,8,20,DarkYellow);
				LCD_DrawRectangle(190,55,8,20,Green);
				recharge1=0;	
				break;
				case 3:
					//tacche batteria iniziale satiety
					LCD_DrawRectangle(160,55,8,20,Red);
					LCD_DrawRectangle(170,55,8,20,Orange);
					LCD_DrawRectangle(180,55,8,20,DarkYellow);
					LCD_DrawRectangle(190,55,8,20,White);
					recharge1=0;
					break;
					case 2:
						//tacche batteria iniziale satiety
						LCD_DrawRectangle(160,55,8,20,Red);
						LCD_DrawRectangle(170,55,8,20,Orange);
						LCD_DrawRectangle(180,55,8,20,White);
						LCD_DrawRectangle(190,55,8,20,White);
						recharge1=0;
						break;
						case 1:
							//tacche batteria iniziale satiety
							LCD_DrawRectangle(160,55,8,20,Red);
							LCD_DrawRectangle(170,55,8,20,White);
							LCD_DrawRectangle(180,55,8,20,White);
							LCD_DrawRectangle(190,55,8,20,White);
							recharge1=0;
							break;
							case 0:
								//tacche batteria iniziale satiety
								LCD_DrawRectangle(160,55,8,20,White);
								LCD_DrawRectangle(170,55,8,20,White);
								LCD_DrawRectangle(180,55,8,20,White);
								LCD_DrawRectangle(190,55,8,20,White);
								recharge1=0;
							break;
		}
	}
	else if(recharge1==1 && selright==1)
	{
		switch(dyno.happiness)
			{
				case 4:
					//tacche batteria inziali happiness
					LCD_DrawRectangle(40,55,8,20,Red);
					LCD_DrawRectangle(50,55,8,20,Orange);
					LCD_DrawRectangle(60,55,8,20,DarkYellow);
					LCD_DrawRectangle(70,55,8,20,Green);
					recharge1=0;
					break;
					case 3:
						//tacche batteria inziali happiness
						LCD_DrawRectangle(40,55,8,20,Red);
						LCD_DrawRectangle(50,55,8,20,Orange);
						LCD_DrawRectangle(60,55,8,20,DarkYellow);
						LCD_DrawRectangle(70,55,8,20,White);
						recharge1=0;
						break;
						case 2:
							//tacche batteria inziali happiness
							LCD_DrawRectangle(40,55,8,20,Red);
							LCD_DrawRectangle(50,55,8,20,Orange);
							LCD_DrawRectangle(60,55,8,20,White);
							LCD_DrawRectangle(70,55,8,20,White);
							recharge1=0;
							break;
							case 1:
								//tacche batteria inziali happiness
								LCD_DrawRectangle(40,55,8,20,Red);
								LCD_DrawRectangle(50,55,8,20,White);
								LCD_DrawRectangle(60,55,8,20,White);
								LCD_DrawRectangle(70,55,8,20,White);
								recharge1=0;
								break;
								case 0:
									//tacche batteria inziali happiness
									LCD_DrawRectangle(40,55,8,20,White);
									LCD_DrawRectangle(50,55,8,20,White);
									LCD_DrawRectangle(60,55,8,20,White);
									LCD_DrawRectangle(70,55,8,20,White);
									recharge1=0;
								break;
			  }
	}
	
	
	
	
  LPC_TIM2->IR = 1;			/* clear interrupt flag */
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
