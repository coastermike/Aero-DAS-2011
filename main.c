#include <p18f4525.h>
#include "pins.h"
#include "init.h"
#include "brakes.h"
#include "adc_read.h"

void main (void)
{
	OSCCON=OSCCON | 0b01110011;  //page 32 datasheet
	Pin_Init();
	Brakes_Init();
	Adc_Init();
	
	
}
