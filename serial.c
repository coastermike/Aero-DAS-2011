#include <p18f4525.h>
#include <usart.h>

void Serial_Init(void)
{
	TRISCbits.TRISC7 = 1;
	TRISCbits.TRISC6 = 0;
	OpenUSART(	USART_TX_INT_OFF & 	
				USART_RX_INT_OFF &
				USART_ASYNCH_MODE &
				USART_EIGHT_BIT &
				USART_BRGH_HIGH, 25);
	
}
