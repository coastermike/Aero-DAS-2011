#include <p18f4525.h>
#include <usart.h>

void Serial_Init(void)
{
	RCSTAbits.SPEN = 1;
	TRISCbits.TRISC7 = 1;
	TRISCbits.TRISC6 = 1;
	
}
	