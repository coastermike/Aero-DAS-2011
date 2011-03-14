#include <p18f4525.h>
#include <usart.h>
#include "pins.h"
#include "init.h"
#include "brakes.h"
#include "adc_read.h"
#include "serial.h"

#pragma config OSC = INTIO67
#pragma config PBADEN = OFF

void main (void)
{
	OSCCON=OSCCON | 0b01110011;  //page 32 datasheet
	Pin_Init();
	Brakes_Init();
	PWMInit();
	Adc_Init();
	Serial_Init();
	Set_Speed(0);
	while(1)
	{
		calibrateNoLoad();
		calibrateLoad();
		if(LED6 && LED7)//start
		{
			takeOff();
		}		
	}

}

//hall effect -> 3.7V to 0V when sensing pin

