#include <p18f4525.h>
#include "pins.h"
#include "init.h"
#include "brakes.h"

void toggle_LED1 (void);
void toggle_BrakeL (void);
void toggle_BrakeR (void);
void hall_L (void);
void hall_R (void);
void HIGH_ISR(void);

const unsigned float constant = .6;
unsigned int TimeIntL = 0, TimeIntR = 0, HallCountL = 0, HallCountR = 0, risetime = 0, falltime = 0;

#pragma code HIGH_INTERRUPT_VECTOR = 0x8	//Where the code goes when a high priority interrupt happens
void high_interrupt (void)
{
	HIGH_ISR();								//separate choosing function...not enough space here
}

#pragma code LOW_INTERRUPT_VECTOR = 0x18	//Where the code goes when a low priority interrupt happens
void low_interrupt (void)
{
	_asm 
	goto toggle_LED1
	_endasm
}
	
#pragma code
#pragma interrupt toggle_LED1			//Interrupt code for status LED
void toggle_LED1(void)
{
	INTCONbits.TMR0IF = 0;
	LEDStatus1 = ~LEDStatus1;
	TMR0H=0x00;
	TMR0L=0x00;//f424
}

#pragma interrupt hall_L		//interrupt code for Left hall sensor
void hall_L(void)
{
	HallCountL++;
	INTCONbits.INT0IF = 0;
}

#pragma interrupt hall_R		//interrupt code for Right hall sensor
void hall_R(void)
{
	HallCountR++;
	INTCON3bits.INT1IF = 0;
}

#pragma interrupt measure_PWM
void measure_PWM(void)
{
	if(CCP1CONbits.CCP1M0 == 1)	//rising
	{
		risetime = CCPR1H;
		risetime = risetime << 8;
		risetime = CCPR1L;
		PIE1bits.CCP1IE = 0;
		CCP1CONbits.CCP1M0 = 0;	//time on falling
		PIR1bits.CCP1IF = 0;
		PIE1bits.CCP1IE = 1;
	}
	else
	{
		falltime = CCPR1H;
		falltime = risetime << 8;
		falltime = CCPR1L;
		PIE1bits.CCP1IE = 0;
		CCP1CONbits.CCP1M0 = 1;	//time on rising
		PIR1bits.CCP1IF = 0;
		PIE1bits.CCP1IE = 1;
		//reset clock counter?
		//Insert code for setting timer1 for the brake here
		
	}	
}
			
#pragma interrupt toggle_BrakeL		//Interrupt code for Left Stepper
void toggle_BrakeL(void)
{
	PIR1bits.TMR1IF = 0;				//Reset the timer1 interrupt bit
	LED3 = ~LED3;
	BrakeL = ~BrakeL;
	TMR1H = (char)(TimeIntL >> 8); 		//Set the initial value for the timer
	TMR1L = (char)TimeIntL; // & 0x00FF) << 8;
}

//#pragma interrupt toggle_BrakeR		//Interrupt code for Right Stepper
//void toggle_BrakeR(void)
//{
//	PIR2bits.TMR3IF = 0;				//Reset the timer3 interrupt bit
//	LED4 = ~LED4;
//	BrakeR = ~BrakeR;
//	TMR3H = (char)(TimeIntR >> 8);
//	TMR3L = (char)TimeIntR;
//}	

void HIGH_ISR (void)				//Figures out what interrupt triggered and runs the code.
{
	if(INTCONbits.INT0IF && INTCONbits.INT0IE)
	{
		_asm
		goto hall_L
		_endasm
	}
	else if (INTCON3bits.INT1IF && INTCON3bits.INT1IE)
	{
		_asm
		goto hall_R
		_endasm
	}
	else if (PIR1bits.CCP1IF && PIE1bits.CCP1IE)
	{
		_asm
		goto measure_PWM
		_endasm
	}	
//	else if (PIR2bits.TMR3IF && PIE2bits.TMR3IE)
//	{
//		_asm
//		goto toggle_BrakeR
//		_endasm
//	}	
	else if(PIR1bits.TMR1IF && PIE1bits.TMR1IE)
	{
		_asm
		goto toggle_BrakeL
		_endasm
	}
}

void Brakes_Init(void)
{
	RCONbits.IPEN = 1;			//enables priority
	IPR1bits.TMR1IP = 1;		//timer1 high priority
	INTCON2bits.TMR0IP = 0;		//timer0 low priority
	IPR2bits.TMR3IP = 1;		//timer3 high priority
	INTCON3bits.INT1IP = 1;		//INT1 high priority
	IPR1bits.CCP1IP = 1;		//CCP1 high priority
	INTCON2bits.INTEDG0 = 0;	//enables INT0 on falling edge
	INTCON2bits.INTEDG1 = 0;	//enables INT1 on falling edge
	INTCONbits.GIEH = 1;		//enables high priority interrupts
	INTCONbits.GIEL = 1;		//enables low priority interrupts
	PIE1bits.TMR1IE = 1;		//enable timer1 interrupt
	INTCONbits.TMR0IE = 1;		//enable timer0 interrupt
	PIE2bits.TMR3IE = 1;		//enable timer3 interrupt
	INTCONbits.INT0IE = 1;		//enable INT0 interrupt
	INTCON3bits.INT1IE = 1;		//enable INT1 interrupt
	CCP1CON = 0b00000101;		//sets up CCP1 as rising edge capture
	PIE1bits.CCP1IE = 1;		//enable CCP1 interrupt -- elsewhere?
	T0CON = 0b00000011;			//sets up timer 0, 1:8 prescalar
	TMR0H = 0x00;				//sets initial as 0
	TMR0L = 0x00;				//
	T1CON = 0b11000000;			//sets up timer 1, 1:1
	TMR1H=0x00;					//sets initial as 0
	TMR1L=0x00;					//
	T3CON = 0b10000000;			//sets up timer3, 1:1 FOR CCP1
	TMR3H = 0x00;				//sets initial as 0
	TMR3L = 0x00;				//
	T0CONbits.TMR0ON = 1;		//Turns timer0 on for the LED status
}

void Set_Speed(unsigned char L)//, unsigned char R) //252 is the fastest safetly.
{
	if (L == 0)
	{
		T1CONbits.TMR1ON = 0;
	}
	else
	{
		TimeIntL = 130 * L + 29871;
		T1CONbits.TMR1ON = 1;
	}
//	if (R == 0)
//	{
//		T3CONbits.TMR3ON = 0;
//	}
//	else
//	{
//		TimeIntR = 130 * R + 29871;
//		T3CONbits.TMR3ON = 1;
//	}
}		

void Reset_Hall_Counts(void)
{
	HallCountR = 0;
	HallCountL = 0;
}

unsigned float get_Hall_L(void)
{
	return HallCountL*constant;
}

unsigned float get_Hall_R(void)
{
	return HallCountR*constant;
}
