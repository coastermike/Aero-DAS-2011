#include "pins.h"
#include <p18f4525.h>

//Sets the Tris bits of the pins that are not part of a peripheral
void Pin_Init()
{
	//setting all Tris bits
	LEDStatus1Tris = 0;
	LEDStatus2Tris = 0;
	
	SW1Tris = 1;
	SW2Tris = 1;
	SW3Tris = 1;
	SW4Tris = 1;
	
	HallLTris = 1;
	HallRTris = 1;
	BrakeLRxTris = 1;
	BrakeRRxTris = 1;
	BrakeLTris = 0;
	BrakeLTris = 0;
	
	LED3Tris = 0;
	LED4Tris = 0;
	LED5Tris = 0;
	LED6Tris = 0;
	LED7Tris = 0;
	LED8Tris = 0;
	LED9Tris = 0;
	LED10Tris = 0;
	
	An0Tris = 1;
	An1Tris = 1;
	
	//setting initial pin state
	LEDStatus1 = 0;
	LEDStatus2 = 0;
	LED3 = 0;
	LED4 = 0;
	LED5 = 0;
	LED6 = 0;
	LED7 = 0;
	LED8 = 0;
	LED9 = 0;
	LED10 = 0;
}	
