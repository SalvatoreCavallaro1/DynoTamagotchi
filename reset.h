#include "GLCD/GLCD.h" 
#include "TouchPanel/TouchPanel.h"
#include "timer/timer.h"
#include "RIT/RIT.h"
#include "joystick/joystick.h"
#include <stdint.h>

static void ResetGame(){
	LCD_DrawLine(5, 5, 5, 315, Black);
	LCD_DrawLine(5, 315, 235, 315, Red);
	LCD_DrawLine(235, 315, 235, 5, Black);
	LCD_DrawLine(235, 5, 5, 5, Black);
	LCD_DrawLine(5, 265, 235, 265, Red);//linea orizzontale
	LCD_DrawLine(120, 265, 120, 315, White); // linea verticale
	GUI_Text(40, 285, (uint8_t *) " Meal  ", White, White);
	GUI_Text(150, 285, (uint8_t *) " Snack  ", White, White);
	GUI_Text(90, 285, (uint8_t *) " RESET  ", Red, White);
	GUI_Text(85, 140, (uint8_t *) "GAME OVER", Red, White);
	enable_timer(1);
}
