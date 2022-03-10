/*
 * Created: 3/15/2019 8:36:18 AM
 * Author : Lawrence
 */ 

#define F_CPU 16000000UL  // 16MHz clock from the debug processor
#include <avr/io.h>
#include <util/delay.h>
#include <math.h>

int Ton;

void uart_init ()
{
	DDRB &= ~(1 << 0) & ~(1 << 1) & ~(1 << 2) & ~(1 << 3); // Set PORT B, Column Pins B0-B3 (input, buttons)
	DDRD |= (1 << 4) | (1 << 5) | (1 << 6) | (1 << 7); // Set PORT D, Row Pins D4-D7 (output)
	DDRC |= (1 << 5); // Set PORT C, pin 5 as an output
	PORTB |= (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3); // Set PORT B, Column Pins as pull-up (default value 1, HIGH)
	PORTD |= (1 << 4) | (1 << 5) | (1 << 6) | (1 << 7);	// Set the row (D4-D7) to 1 (HIGH)
	TCCR0A |= (1 << WGM01); // Set Timer 0 to CTC mode
	TCCR0B |= (1 << CS02) | (1 << CS00); // Set pre-scale value of 1024 and start the timer
}

int calculate_freq (unsigned int KEYCHAR)
{
float period;

	period = 1.0 / (440.0 * (pow( (pow(2.0, (1.0/12.0))), KEYCHAR))); // Time Period (Frequency) Formula

return period * 10000.0;
}

/*void delay_func (int x)
{
	for (int i = 0; i <= x; i++)
	{
		_delay_us(1);
	}
}
*/

void delay_B (int Ton)
{
	OCR0B = (Ton/2); // Set OCR0B to half of OCR0A (OCR0a = Ton/Toff) (50% duty cycle)
	while ((TIFR0 & (1 << OCF0B)) == 0); // Wait for the overflow at the bottom value (counter to reach 0CR0B value)... break while loop when OCF0B = 1
	TIFR0 |= (1 << OCF0B); // Reset overflow flag at 0CF0B bit

return;
}

void delay_A (int Ton)
{
	OCR0A = Ton; // Set top value OCR0A value to frequency calculated
	while ((TIFR0 & (1 << OCF0A)) == 0); // Wait for the overflow at the top value (counter to reach 0CR0A value)... end while loop when OCF0A = 1
	TIFR0 |= (1 << OCF0A); // Reset overflow flag at OCF0A bit
	
return;
}


int main(void)
{
	uart_init();
	unsigned int KEYCHAR[4][4] =  { {0, 1, 2, 3},
                                	{4, 5, 6, 7},
	                                {8, 9, 10, 11},
	                                {12, 13, 14, 15} }; // Length of 4 (rows), with 4 elements (column) inside of it
//	int Ton;
	while (1)
	{
		
		for (int i = 0; i < 4; i++)  // Set each row to LOW one row at a time. We are going to scan this for any inputs (column)
		{
			PORTD &= ~(1 << (4+i)); // Set row (D4-D7) to 0 (LOW)
			
			for (int j = 0; j < 4; j++) // While you are scanning row D(4+i), check if any column in this row is pressed
			{
				if (!(PINB & (1 << j))) // Reads value at PINB#j and checks if 0 (pressed)
				{
					Ton = calculate_freq(KEYCHAR[i][j]); // Set float period value to integer Ton
					// The integer value at the button pressed is sent to frequency calculator function.
					
// 					OCR0A = Ton; // Set top value OCR0A value to frequency calculated
// 					OCR0B = (Ton/2); // Set OCR0B to half of OCR0A (50% duty cycle)
					
//                  Ton = Toff 
					
					PORTC |= (1 << 5); // Turn on the sound
					delay_B(Ton);
 					PORTC &= ~(1 << 5); // Turn off the sound
					delay_A(Ton); // We can set parameter to Ton because in this case.. Toff = Ton

				}
			} 
			PORTD |= (1 << (4+i)); // Set row (D4-D7) back to 1 (HIGH) to repeat the process
		}
	}
				
}
									



