#include <stdint.h>

typedef struct {
     int age;
		 int state;
		 int satiety;
		 int happiness;
     } character;

		 
//STATE
		 /*
		 0->IDLE
		 1->WALK RIGHT
		 2->WALK LEFT
		 3->RUN AWAY
		 */	
		 
 static character dyno={-1,0,4,4};
