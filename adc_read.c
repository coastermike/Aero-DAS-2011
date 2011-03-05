#include "pins.h"
#include <p18f4525.h>
#include "adc_read.h"

char adc_Convert(char raw);

void Adc_Init()
{
	ADCON1=0x07; //Sets VSS and VDD voltage ref, sets AN0 through AN7 as analog.
	ADCON2=0b00010101; //Sets Left Justified; 2 TAD; Fosc/16
}

unsigned char Adc_Read(char ch)  //return value read?
{	
	switch (ch)			//Selects ADCON0 based on the desired channel.
	{
		case 0: ADCON0=0b00000000; 	//AN0; Force L
			break;
		case 1: ADCON0=0b00000100;	//AN1; Force R
			break;
	}	
	ADCON0 |= 0x01;					//Turns on A/D converter
	ADCON0 |= 0b00000010;			//starts the conversion
	while(ADCON0bits.GO == 0b1){}		//waits until the conversion is done
	
	ADCON0 = 0x00;			//disables the converter
	return ADRESH;					//returns the result
}
