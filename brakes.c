#include <p18f4525.h>
#include "pins.h"
#include "init.h"
#include "brakes.h"
#include "adc_read.h"

void toggle_LED1 (void);
void toggle_BrakeL (void);
void toggle_BrakeR (void);
void hall_L (void);
void hall_R (void);
void HIGH_ISR(void);

const unsigned float constant = .6;
unsigned int TimeIntL = 0, TimeIntR = 0, TimeIntClk = 0, HallCountL = 0, HallCountR = 0, TempHallCountL = 0, TempHallCountR = 0, risetime = 0, falltime = 0;
unsigned char overflowCount = 0;
unsigned char history = 0, history1 = 0, save = 0;
unsigned int noLoadL = 0, noLoadR = 0, LoadL = 0, LoadR = 0;

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
	PIR1bits.TMR2IF = 0;
	overflowCount++;
	if(overflowCount > 14)
	{
		LEDStatus1 = ~LEDStatus1;
		overflowCount = 0;
	}
}

#pragma interrupt toggle_LED2
void toggle_LED2(void)
{
	LEDStatus2 = ~LEDStatus2;
	if (history == 1)
	{
		history = 2;
	}
	else if (history1 == 1)
	{
		history1 = 2;
	}
	else if (history1 == 10)
	{
		history1 = 11;
	}
	else if (save == 2)
	{
		save = 3;
	}	
	T0CONbits.TMR0ON = 0;
	INTCONbits.TMR0IF = 0;
	TMR0H = (char)(TimeIntClk >> 8); 		//Set the initial value for the timer
	TMR0L = (char)TimeIntClk; // & 0x00FF) << 8;
}
		
#pragma interrupt hall_L		//interrupt code for Left hall sensor
void hall_L(void)
{
	if(save == 1)
	{
		HallCountL++;
	}
	else if (save == 2)
	{
		TempHallCountL++;
	}		
	INTCONbits.INT0IF = 0;
}

#pragma interrupt hall_R		//interrupt code for Right hall sensor
void hall_R(void)
{
	if(save == 1)
	{
		HallCountR++;
	}
	else if (save == 2)
	{
		TempHallCountR++;
	}
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
	else if(PIR1bits.TMR2IF && PIE1bits.TMR2IE)
	{
		_asm 
		goto toggle_LED1
		_endasm
	}
	else if(INTCONbits.TMR0IE && INTCONbits.TMR0IF)
	{
		_asm
		goto toggle_LED2
		_endasm
	}	
}

void Brakes_Init(void)
{
	RCONbits.IPEN = 1;			//enables priority
	IPR1bits.TMR1IP = 1;		//timer1 high priority
	INTCON2bits.TMR0IP = 1;		//timer0 high priority
	IPR2bits.TMR3IP = 1;		//timer3 high priority
	IPR1bits.TMR2IP = 1;		//timer2 low priority
	INTCON3bits.INT1IP = 1;		//INT1 high priority
	IPR1bits.CCP1IP = 1;		//CCP1 high priority
	INTCON2bits.INTEDG0 = 0;	//enables INT0 on falling edge
	INTCON2bits.INTEDG1 = 0;	//enables INT1 on falling edge
	INTCONbits.GIEH = 1;		//enables high priority interrupts
	INTCONbits.GIEL = 1;		//enables low priority interrupts
	PIE1bits.TMR1IE = 1;		//enable timer1 interrupt
	INTCONbits.TMR0IE = 1;		//enable timer0 interrupt
	PIE2bits.TMR3IE = 1;		//enable timer3 interrupt
	PIE1bits.TMR2IE = 1;		//enable tiemr2 interrupt
	INTCONbits.INT0IE = 1;		//enable INT0 interrupt
	INTCON3bits.INT1IE = 1;		//enable INT1 interrupt
	CCP1CON = 0b00000101;		//sets up CCP1 as rising edge capture
	PIE1bits.CCP1IE = 1;		//enable CCP1 interrupt -- elsewhere?
	T0CON = 0b00000111;			//sets up timer 0, 1:256 prescalar
	TMR0H = 0xE1;				//sets initial as 0
	TMR0L = 0x7B;				//
	T1CON = 0b11000000;			//sets up timer 1, 1:1
	TMR1H=0x00;					//sets initial as 0
	TMR1L=0x00;					//
	T3CON = 0b10000000;			//sets up timer3, 1:1 FOR CCP1
	TMR3H = 0x00;				//sets initial as 0
	TMR3L = 0x00;				//
	T2CON = 0b01111011;			//sets up timer2, 1:256 for LED status
	PR2=0xFF;
	T2CONbits.TMR2ON = 1;		//Turns timer2 on for the LED status
//	T0CONbits.TMR0ON = 1;
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

//waits for SW1 or 2 to be pressed for >1sec, then stores the values while unloaded.
void calibrateNoLoad(void)
{
	if((SW1 || SW2) && history == 0)
	{
		TMR0H = 0xE1;
		TMR0L = 0x7B;
		T0CONbits.TMR0ON = 1;
		history = 1;
	}
	if(history == 2)
	{
		while(SW1 || SW2){}
		noLoadL = Adc_Read(0);
		noLoadR = Adc_Read(1);
		LED6 = ~LED6;
		history = 0;
	}
	if(!SW1 && !SW2 && history == 1)
	{
		history = 0;
		T0CONbits.TMR0ON = 0;
	}		
}

//waits for SW3 or 4 to be pressed for >1sec, then waits one second and then stores the force values while loaded
void calibrateLoad(void)
{
	if((SW3 || SW4) && history1 == 0)
	{
		TMR0H = 0xE1;
		TMR0L = 0x7B;
		T0CONbits.TMR0ON = 1;
		history1 = 1;
	}
	if(history1 == 2)
	{
		while(SW3 || SW4){}
		TMR0H = 0xE1;
		TMR0L = 0x7B;
		T0CONbits.TMR0ON = 1;
		history1 = 10;
	}
	if(history1 == 11)
	{	
		LoadL = Adc_Read(0);
		LoadR = Adc_Read(1);
		LED7 = ~LED7;
		history1 = 0;	
	}
	if(!SW3 && !SW4 && history1 == 1)
	{
		history1 = 0;
		T0CONbits.TMR0ON = 0;
	}		
}

//takeoff hall sensor sequence, if the plane loses contact for >2 seconds stop counting
void takeOff(void)
{
	if(save == 0)
	{
		save = 1;
		INTCONbits.INT0IE = 1;
		INTCON3bits.INT1IE = 1;
	}	
	if((Adc_Read(0) > LoadL) && (Adc_Read(1) > LoadR) && save == 1)
	{
		TMR0H = 0xC2;		//2 seconds
		TMR0L = 0xF7;
		save = 2;
		T0CONbits.TMR0ON = 1;
	}
	if(((Adc_Read(0) < LoadL) || (Adc_Read(1) < LoadR)) && save == 2)
	{
		T0CONbits.TMR0ON = 0;
		save = 1;
		HallCountL = HallCountL + TempHallCountL;
		HallCountR = HallCountR + TempHallCountR;
	}	
}

void land(void)
{
	
}
		