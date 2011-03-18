#include <p18f4525.h>
#include <usart.h>

void Serial_Init(void)
{
	RCSTAbits.SPEN=1;
	TRISCbits.TRISC7 = 1;
	TRISCbits.TRISC6 = 1;
	OpenUSART(	USART_TX_INT_OFF & 	
				USART_RX_INT_OFF &
				USART_ASYNCH_MODE &
				USART_EIGHT_BIT &
				USART_SINGLE_RX &
				USART_BRGH_HIGH, 25);
	BAUDCON = 0b01000000;
	
}
