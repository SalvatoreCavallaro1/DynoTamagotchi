#include "GLCD/GLCD.h" 
#include "TouchPanel/TouchPanel.h"
#include "timer/timer.h"
#include "RIT/RIT.h"
#include "joystick/joystick.h"
#include <stdint.h>



static void InitGame(){
	LCD_Initialization();
	LCD_Clear(White);
	
	LCD_DrawLine(5, 5, 5, 315, Black);
	LCD_DrawLine(5, 315, 235, 315, Black);
	LCD_DrawLine(235, 315, 235, 5, Black);
	LCD_DrawLine(235, 5, 5, 5, Black);
	LCD_DrawLine(5, 265, 235, 265, Black);//linea orizzontale
	LCD_DrawLine(120, 265, 120, 315, Black); // linea verticale
	GUI_Text(40, 285, (uint8_t *) " Meal  ", Black, White);
	GUI_Text(150, 285, (uint8_t *) " Snack  ", Black, White);
	GUI_Text(15, 30, (uint8_t *) " Happiness  ", Black, White);
	GUI_Text(145, 30, (uint8_t *) " Satiety  ", Black, White);
	//batteria happiness
	LCD_DrawLine(35, 50, 35, 80, Black); //verticale sx
	LCD_DrawLine(35, 80, 85, 80, Black); // orizzontale bassa
	LCD_DrawLine(85, 80, 85, 50, Black); // verticale dx
	LCD_DrawLine(85, 50, 35, 50, Black); // orizontale alta
	//"contatto" batteria happiness
	LCD_DrawLine(90, 70, 90, 60, Black); // orizontale alta
	LCD_DrawLine(89, 70, 89, 60, Black); // orizontale alta
	LCD_DrawLine(88, 70, 88, 60, Black); // orizontale alta
	LCD_DrawLine(87, 70, 87, 60, Black); // orizontale alta
	LCD_DrawLine(86, 70, 86, 60, Black); // orizontale alta
	LCD_DrawLine(85, 70, 85, 60, Black); // orizontale alta

	//batteria satiety
	LCD_DrawLine(155, 50, 155, 80, Black); //verticale sx
	LCD_DrawLine(155, 80, 205, 80, Black); // orizzontale bassa
	LCD_DrawLine(205, 80, 205, 50, Black); // verticale dx
	LCD_DrawLine(205, 50, 155, 50, Black); // orizontale alta
	//"contatto" satiety
	LCD_DrawLine(210, 70, 210, 60, Black); // verticale dx
	LCD_DrawLine(209, 70, 209, 60, Black); // verticale dx
	LCD_DrawLine(208, 70, 208, 60, Black); // verticale dx
	LCD_DrawLine(207, 70, 207, 60, Black); // verticale dx
	LCD_DrawLine(206, 70, 206, 60, Black); // verticale dx
	LCD_DrawLine(205, 70, 205, 60, Black); // verticale dx
	
	
	joystick_init();											/* Joystick Initialization     */
	init_timer(0,0x17D7840); 		      	  /* 1s * 25MHz = 25*10^6 = 0x17D7840 */ //animazioni character
	enable_timer(0);
	init_timer(1,0x004C4B40);             //joystick
	enable_timer(1);
	init_timer(2,0x17D7840); 		      	  /* 1s * 25MHz = 25*10^6 = 0x17D7840 */  //animazioni age,happiness satiety
	enable_timer(2);
}

