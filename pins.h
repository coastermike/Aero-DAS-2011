#ifndef PINS_H
#define PINS_H

//defines pins as names

//Format of code (* represents any name of pin)
//	* gives a name to the specified pin so that * can be set or read in the program
//	*Tris sets the Tris bit for the pin. 0 is output, 1 is input.

//Status LEDs
#define LEDStatus1 PORTAbits.RA4
#define LEDStatus1Tris TRISAbits.TRISA4
#define LEDStatus2 PORTCbits.RC0
#define LEDStatus2Tris TRISCbits.TRISC0

//Switches
#define SW1 PORTBbits.RB2
#define SW1Tris TRISBbits.TRISB2
#define SW2 PORTBbits.RB3
#define SW2Tris TRISBbits.TRISB3
#define SW3 PORTBbits.RB4
#define SW3Tris TRISBbits.TRISB4
#define SW4 PORTBbits.RB5
#define SW4Tris TRISBbits.TRISB5

#define HallL PORTBbits.RB0
#define HallLTris TRISBbits.TRISB0
#define HallR PORTBbits.RB1
#define HallRTris TRISBbits.TRISB1

//Analog Tris settings
#define An0Tris TRISAbits.TRISA0
#define An1Tris TRISAbits.TRISA1

//PWM
#define BrakeLRx PORTCbits.RC2
#define BrakeLRxTris TRISCbits.TRISC2
#define BrakeRRx PORTCbits.RC1
#define BrakeRRxTris TRISCbits.TRISC1

#define BrakeL PORTAbits.RA6
#define BrakeR PORTAbits.RA7
#define BrakeLTris TRISAbits.TRISA6
#define BrakeRTris TRISAbits.TRISA7

//PORTD - Debug LEDS
#define LED3 PORTDbits.RD0
#define LED3Tris TRISDbits.TRISD0
#define LED4 PORTDbits.RD1
#define LED4Tris TRISDbits.TRISD1
#define LED5 PORTDbits.RD2
#define LED5Tris TRISDbits.TRISD2
#define LED6 PORTDbits.RD3
#define LED6Tris TRISDbits.TRISD3
#define LED7 PORTDbits.RD4
#define LED7Tris TRISDbits.TRISD4
#define LED8 PORTDbits.RD5
#define LED8Tris TRISDbits.TRISD5
#define LED9 PORTDbits.RD6
#define LED9Tris TRISDbits.TRISD6
#define LED10 PORTDbits.RD7
#define LED10Tris TRISDbits.TRISD7

#endif	
