#include <p18f4525.h>
#include <usart.h>
#include "pins.h"
#include "init.h"
#include "brakes.h"
#include "adc_read.h"

#pragma config OSC = INTIO67
#pragma config PBADEN = OFF

void main (void)
{
	unsigned char i;
	OSCCON=OSCCON | 0b01110011;  //page 32 datasheet
	Pin_Init();
	Brakes_Init();
	Adc_Init();
	Set_Speed(0);
	while(1)
	{
		i = Adc_Read(0);
	}

}

//hall effect -> 3.7V to 0V when sensing pin

