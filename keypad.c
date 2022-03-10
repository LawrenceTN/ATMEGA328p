/*
 * Author : Lawrence
 */ 

#define F_CPU 16000000UL  // 16MHz clock from the debug processor
#define BAUD 9600
#define BAUDRATE ((F_CPU)/(BAUD*16UL)-1) // Async Normal Mode
#include <avr/io.h>
#include <util/delay.h>

void uart_transmit (unsigned char KEYCHAR)
{
	while (!( UCSR0A & (1<<UDRE0))); // Wait until buffer is empty (character reaches the terminal) before sending next character. While UDRE0 = 1, it is empty.

	 UDR0 = KEYCHAR; // Set load data register to next char
	_delay_ms(50);

}

void uart_init ()
{
	/* UBRR IS 12-BITS  */
	UBRR0H = (BAUDRATE>>8); // Shift by 8 to the right clear it and to set HIGH BIT REGISTER to 0
	UBRR0L = (BAUDRATE); // Set low bit register to the calculated BAUDRATE
	
	UCSR0C |= (1<<UCSZ00) | (1<<UCSZ01); // Setting up 8-bit data frame and stop bit in USCRC register. Stop bit (USBSn) is initially 0, so no change
	UCSR0B |= (1<<TXEN0) | (1<<RXEN0); // Enable transmitter & receiver in UCSRB register
	
	DDRB &= ~(1 << 0) & ~(1 << 1) & ~(1 << 2) & ~(1 << 3); // Set PORT B, Column Pins B0-B3 (input, buttons)
	DDRD |= (1 << 4) | (1 << 5) | (1 << 6) | (1 << 7); // Set PORT D, Row Pins D4-D7 (output)
	PORTB |= (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3); // Set PORT B, Column Pins as pull-up (default value 1, HIGH)
	PORTD |= (1 << 4) | (1 << 5) | (1 << 6) | (1 << 7);	// Set the row (D4-D7) to 1 (HIGH)
}


int main(void)
{
    uart_init();
	unsigned char KEYCHAR[4][4] =  { {'1', '2', '3', 'A'},
		         				    {'4', '5', '6', 'B'},
				         		    {'7', '8', '9', 'C'},
						            {'*', '0', '#', 'D'} }; // Length of 4 (rows), with 4 elements (column) inside of it
		
    while (1) 
    {
		
	for (int i = 0; i < 4; i++)  // Set each row to LOW one row at a time. We are going to scan this for any inputs (column)
		{
			PORTD &= ~(1 << (4+i)); // Set row (D4-D7) to 0 (LOW)
			
			for (int j = 0; j < 4; j++) // While you are scanning row D(4+i), check if any column in this row is pressed
			{
				if (!(PINB & (1 << j))) // Reads value at PINB#j and checks if 0 (pressed)
				{
						uart_transmit(KEYCHAR[i][j]); // Transmit the character 
						_delay_ms(100);
				}	
			}
			PORTD |= (1 << (4+i)); // Set row (D4-D7) back to 1 (HIGH) to repeat the process
		}
	}
	
} 
