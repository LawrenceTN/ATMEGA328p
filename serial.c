/*
 * Author : Lawrence
 */

/*
DESCRIPTION: SET TO ASYNCHRONOUS USART MODE, SET UP 8 BIT DATA FRAME AND ENABLE USART TRANSMITTER.
SET UCSZ01 AND UCSZ00 TO '1' FOR 8-BIT DATA FRAME AND SET '0' TO USBS FOR 1-BIT STTOP BIT.
CHECKPOINT: TRANSMIT RED ID USING USART, DISPLAY OUTPUT ON THE ATMEL STUDIO DATA VISUALIZER, RED #ID SHOULD BE FOLLOWED BY A NEWLINE, DISPLAYED ON INFINITE LOOP.
*/

#define F_CPU 16000000UL  // 16MHz clock from the debug processor
#include <avr/io.h>
#include <util/delay.h>
#define BAUD 9600
#define BAUDRATE ((F_CPU)/(BAUD*16UL)-1) // Async Normal Mode

void uart_transmit (unsigned char redID)
{
		while (!( UCSR0A & (1<<UDRE0))); // Wait until buffer is empty (character reaches the terminal) before sending next character. While UDRE0 = 1, it is empty.

			UDR0 = redID; // Set load data register to next char in array
			_delay_ms(50);

}

void uart_init ()
	{
		/* UBRR IS 12-BITS  */
		UBRR0H = (BAUDRATE>>8); // Shift by 8 to the right clear it and to set HIGH BIT REGISTER to 0
		UBRR0L = (BAUDRATE); // Set low bit register to the calculated BAUDRATE
		
		UCSR0C |= (1<<UCSZ00) | (1<<UCSZ01); // Setting up 8-bit data frame and stop bit in USCRC register. Stop bit (USBSn) is initially 0, so no change
		UCSR0B |= (1<<TXEN0) | (1<<RXEN0); // Enable transmitter & receiver in UCSRB register
	}
	
int main(void)
{

	uart_init();
	
    while (1) 
    {
			unsigned char redID[] = "821432515\r\n"; // Size 9
			for (int i = 0; i < 11; i++) // Include the \r and \n characters 
			{
				uart_transmit(redID[i]);
			}
							
    }
	return 0;
}

