/*
 * Author : Lawrence
 */ 

#define F_CPU 16000000UL // 16 Mhz Clock from Debug Processor
#include <avr/io.h>

void uart_init()
{
			DDRB &= ~(1<<DDB7); // Set PORT B, Pin 7 to 0 (input)
			DDRB |= (1<<DDB5); // Setting the LED at bit PORTB5 to an output register
			PORTB |= (1<<7); // Pull-up input 
			TCCR1B |= (1 << CS11) | (1 << WGM12); // Uses Timer 1 (16-bit) and sets the pre-scale value to '8', set to CTC mode and starts the timer. Top = OCR1A  
			OCR1A = 19999; // Sets the max value of the timer (10 ms) 
			OCR1B = 19999; // 100% Duty Cycle (Half if you want 50% (9999), Quarter if you want 25%, etc 
}


int main(void)
{

uart_init();

		while (1)
		{	
			
			if (!(PINB & (1 << 7))) // If button pressed down (0)...
			{
				if (TCNT1 == OCR1B) // If the timer1 count reaches 19999 (100% Duty Cycle), turn on LED
				{
					PORTB |= (1 << 5); // LED turns on
				}
				
				if (TIFR1 & (1 << OCF1A)) // (Timer interrupt flag register at OCF1A bit) 
										  // If the overflow flag is set to 1 from the compare of TCNT1 and OCR1B, toggle LED and clear the flag 
				{
					if (OCR1B <= 50) // 50 is an arbitrary number to set the speed of the blink when adjusting brightness.
					{
						PORTB |= (1 << 5); // Let the LED stay on.
					}
					else 
					{
						PORTB &= ~(1 << 5); // Turn it off
					}
					
					TIFR1 |= (1 << OCF1A); // Reset the overflow flag register bit by writing logic '1' onto it. NOT ZERO BECAUSE CIRCUIT DESIGNED THIS WAY. 
					TCNT1 = 0; // Reset the counter of Timer 1
					OCR1B = OCR1B - 50; // Decrement duty cycle which increases the time the LED is on.
				}
				
				if (OCR1B <= 50) // Since we decrement the OCR1B duty cycle by 50, 
				                 // we do this to keep the register from overflowing and restarting from 0.
				{
					OCR1B = 50;  // Setting the Output Compare B to 50 means the LED will stay on.
				}
			}
				
			else // When the button is not pressed (1), begin to dim down. Reverse of above. 
			{
				if (TCNT1 == OCR1B) // If the timer1 count reaches OCR1B value, turn on LED
				{
					PORTB |= (1 << 5);
				}
				
				if (TIFR1 & (1 << OCF1A)) // (Timer interrupt flag register at OCF1A bit)
						     			  // If the overflow flag is set to 1 from the compare of TCNT1 and OCR1B, turn off LED and clear the flag
				{
					PORTB &= ~(1 << 5); // Led OFF
					TIFR1 |= (1 << OCF1A); // Clear the flag register register at bit OCF1A by writing 1 on to it
					TCNT1 = 0;  // Resets the counter of the timer.
					OCR1B = OCR1B + 50; // Instead of now decrementing the duty cycle, we will increment it.
				}
				
				if (OCR1B >= (OCR1A)) // When we increase each time by 50 as it dims down, Output B compares with Output A and if it is more than A, Set B to A.
				{
					OCR1B = OCR1A; // Set Output B to Output A so that it will remain off when blinked down.
				}
			}
		}
}
				
