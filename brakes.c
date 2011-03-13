#include <p18f4525.h>
#include <pwm.h>
#include <capture.h>
#include "pins.h"
#include "init.h"
#include "brakes.h"
#include "adc_read.h"

void toggle_LED1 (void);
void toggle_LED2 (void);
void toggle_BrakeR (void);
void hall_L (void);
void hall_R (void);
void PWMRead(void);
void Timer3Reset(void);
void HIGH_ISR(void);

const unsigned float constant = .6;
unsigned int TimeIntClk = 0, HallCountL = 0, HallCountR = 0, TempHallCountL = 0, TempHallCountR = 0, risetime = 0, falltime = 0;
unsigned char overflowCount = 0, leftForceRead = 0, rightForceRead = 0;
unsigned char history = 0, history1 = 0, save = 0, pwmRiseFall = 0, timer3H = 0, timer3L = 0;
unsigned int noLoadL = 0, noLoadR = 0, LoadL = 0, LoadR = 0, pulse = 0, timer3 = 0;

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
	PIR1bits.TMR1IF = 0;
	overflowCount++;
	if(overflowCount > 1)
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
	else if (save == 4)
	{
		save = 5;
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
	else if (save == 2 || save == 4)
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
	else if (save == 2 || save == 4)
	{
		TempHallCountR++;
	}
	INTCON3bits.INT1IF = 0;
}

#pragma interrupt PWMRead
void PWMRead(void)
{
	if(pwmRiseFall == 0)
	{
		INTCON2bits.INTEDG2 = 0;	//falling
		INTCON3bits.INT2IF = 0;
		TMR3H = 0x00;				//sets initial as 0
		TMR3L = 0x00;
		T3CONbits.TMR3ON = 1;
		pwmRiseFall = 1;
	}
	else if(pwmRiseFall == 1)
	{
		INTCON2bits.INTEDG2 = 1;	//rising
		T3CONbits.TMR3ON = 0;
		timer3 = ((int)timer3H << 8 ) | timer3L;
		INTCON3bits.INT2IF = 0;
		pwmRiseFall = 0;
		if(timer3 < 1)
		{
			Set_Speed(0);
		}
		else
		{
			Set_Speed(timer3);
		}	
	}	
}
	
#pragma interrupt Timer3Reset
void Timer3Reset(void)
{
	PIR2bits.TMR3IF = 0;
}
	
//#pragma interrupt measure_PWM
//void measure_PWM(void)
//{
//	if(CCP1CONbits.CCP1M0 == 1)	//rising
//	{
//		risetime = CCPR1H;
//		risetime = risetime << 8;
//		risetime = CCPR1L;
//		PIE1bits.CCP1IE = 0;
//		CCP1CONbits.CCP1M0 = 0;	//time on falling
//		PIR1bits.CCP1IF = 0;
//		PIE1bits.CCP1IE = 1;
//	}
//	else
//	{
//		falltime = CCPR1H;
//		falltime = risetime << 8;
//		falltime = CCPR1L;
//		PIE1bits.CCP1IE = 0;
//		CCP1CONbits.CCP1M0 = 1;	//time on rising
//		PIR1bits.CCP1IF = 0;
//		PIE1bits.CCP1IE = 1;
//		TMR3H = 0x00;		//reset timer to 0
//		TMR3L = 0x00;
//		pulse = falltime-risetime;
//	
//		pulse = ReadCapture1();
//		PIR1bits.CCP1IF = 0;
//		PIR2bits.TMR3IF = 0;
//		if(pulse < 2100)
//		{
//			Set_Speed(0);
//		}
//		else
//		{
//			Set_Speed(pulse);
//		}	
////Insert code for setting timer1 for the brake here
//	
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
//	else if (PIR1bits.CCP1IF && PIE1bits.CCP1IE)
//	{
//		_asm
//		goto measure_PWM
//		_endasm
//	}
	else if (INTCON3bits.INT2IF && INTCON3bits.INT2IE)
	{
		_asm
		goto PWMRead
		_endasm
	}	
	else if (PIR2bits.TMR3IF && PIE2bits.TMR3IE)
	{
		_asm
		goto Timer3Reset
		_endasm
	}	
	else if(PIR1bits.TMR1IF && PIE1bits.TMR1IE)
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
	INTCON3bits.INT1IP = 1;		//INT1 high priority (INT0 is always high)
	INTCON3bits.INT2IP = 1;		//INT2 high priority
//	IPR1bits.CCP1IP = 1;		//CCP1 high priority
	INTCON2bits.INTEDG0 = 0;	//enables INT0 on falling edge
	INTCON2bits.INTEDG1 = 0;	//enables INT1 on falling edge
	INTCON2bits.INTEDG2 = 1;	//enables INT2 on rising edge
	INTCONbits.GIEH = 1;		//enables high priority interrupts
	INTCONbits.GIEL = 1;		//enables low priority interrupts
	PIE1bits.TMR1IE = 1;		//enable timer1 interrupt
	INTCONbits.TMR0IE = 1;		//enable timer0 interrupt
	PIE2bits.TMR3IE = 1;		//enable timer3 interrupt
	INTCONbits.INT0IE = 1;		//enable INT0 interrupt
	INTCON3bits.INT1IE = 1;		//enable INT1 interrupt
	INTCON3bits.INT2IE = 1;		//enables INT2 interrupt
//	CCP1CON = 0b00000101;		//sets up CCP1 as rising edge capture
//	PIE1bits.CCP1IE = 1;		//enable CCP1 interrupt -- elsewhere?
//	OpenCapture1(CAPTURE_INT_ON & CAP_EVERY_RISE_EDGE);
	T0CON = 0b00000111;			//sets up timer 0, 1:256 prescalar
	TMR0H = 0xE1;				//sets initial as 0
	TMR0L = 0x7B;				//
	T1CON = 0b11110000;			//sets up timer 1, 1:8 for LED status
	TMR1H=0x0B;					//sets initial as 0
	TMR1L=0xF7;					//
//	T3CON = 0b11110000;			//sets up timer3, 1:8 FOR CCP1
	T3CONbits.RD16 = 1;
	T3CONbits.T3CKPS1 = 0b11;
	T3CONbits.TMR3CS = 0;
	TMR3H = 0x00;				//sets initial as 0
	TMR3L = 0x00;				//
	T1CONbits.TMR1ON = 1;		//Turns timer1 on for the LED status
//	T3CONbits.TMR3ON = 1;
}

void PWMInit(void)
{
	OpenPWM2(0xFF);
}
	
void Set_Speed(unsigned char L)//, unsigned char R) //252 is the fastest safetly.
{
	if (L == 0)
	{
		SetDCPWM2(0x000);
	}
	else
	{
		SetDCPWM2((int)(-630+3*L/10));
	}
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
	if((SW2) && history == 0)
	{
		TMR0H = 0xE1;
		TMR0L = 0x7B;
		T0CONbits.TMR0ON = 1;
		history = 1;
	}
	if(history == 2)
	{
		while(SW2){}
		noLoadL = Adc_Read(0);
		noLoadR = Adc_Read(1);
		LED6 = ~LED6;
		history = 0;
	}
	if(!SW2 && history == 1)
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
	leftForceRead = Adc_Read(0);
	rightForceRead = Adc_Read(1);
	if(save == 0)
	{
		save = 1;
		INTCONbits.INT0IE = 1;
		INTCON3bits.INT1IE = 1;
	}	
	if((leftForceRead > LoadL) && (rightForceRead > LoadR) && save == 1)	//wheels lift
	{
		TMR0H = 0xC2;		//2 seconds
		TMR0L = 0xF7;
		save = 2;
		T0CONbits.TMR0ON = 1;
	}
	if(((leftForceRead < LoadL) || (rightForceRead < LoadR)) && save == 2)	//wheels retouch downwithin 2 seconds
	{
		T0CONbits.TMR0ON = 0;
		save = 1;
		HallCountL = HallCountL + TempHallCountL;
		HallCountR = HallCountR + TempHallCountR;
	}
	if(((leftForceRead < LoadL) || (rightForceRead < LoadR)) && save == 3)	//wheels touch for landing
	{
		TMR0H = 0xF0;
		TMR0L = 0xBE;
		save = 4;
		T0CONbits.TMR0ON = 1;
	}
	if(((leftForceRead > LoadL) || (rightForceRead > LoadR)) && save == 4)	//plane in air following initial touch /error prevention
	{
		T0CONbits.TMR0ON = 0;
		save = 3;
		HallCountL = HallCountL + TempHallCountL;
		HallCountR = HallCountR + TempHallCountR;
	}
	
}
