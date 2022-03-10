/*
 * Author : Lawrence
 */ 

#include <avr/io.h>
#define F_CPU 16000000UL
#include <avr/interrupt.h>
#include <math.h>
#include <util/delay.h>

unsigned int KEYCHAR[4][4] = { {1, 2, 3, 0}, 
							   {4 ,5, 6, 0}, 
							   {7, 8, 9, 0},
							   {0, 0, 0, 0}
							 };

// This 4x4 global int array represents our keypad. Anything other than 1-9 is considered a 0% Duty Cycle

void uart_init()
{
	DDRB |= (1 << DDB5); // Sets B5 (LED) as an output (1)
	DDRD |= (1 << DDB4) | (1 << DDD5) | (1 << DDD6) | (1 << DDD7); // Sets the rows of keypad as outputs (1)
	DDRB &= ~((1 << DDB0) | (1 << DDB1) | (1 << DDB2) | (1 << DDB3)); // Sets the columns of keypad as inputs (0)
	PORTB |= (1 << PORTB0) | (1 << PORTB1) | (1 << PORTB2) | (1 << PORTB3); // Enable pull ups for the columns (inputs) (1)
	PORTD |= (1 << 5) | (1 << 6) | (1 << 7); // Set the rows to 1 (HIGH) to prepare for scan
}

void timer0 (int button) // Timer function that sets up and calculates the duty cycle based on the variable from the pressed button
{
	// RedID: #821432515
	TCCR0A |= (1 << WGM01); // Set Timer0 to CTC mode used for LED waveform
	TCCR0B |= (1 << CS02); // Set the pre-scaler to 256
	OCR0A = 103; // PWM frequency = (5+1)*100 = 600Hz => 1/600 = 1.667ms => 104 => 103	
	OCR0B = (OCR0A * button) / 10; // Calculates the percentage of duty cycle from the keypad button
	TIMSK0 |= (1 << OCIE0A) | (1 << OCIE0B); // Sets ISR CompA and ISR compB vector by setting 1 to Timer Interrupt Mask Register for Timer 0 at bit 1 and 2 
}


void keypad_scan ()
{
			for (int i = 0; i < 4; i++)
			{
				PORTD &= ~(1 << (4+i)); // Set each input row to low and then check each column one at a time
				for (int j = 0; j < 4; j++) // While you are scanning row D(4+i), check if any column in this row is pressed
				{
					if (!(PINB & (1 << j))) // Reads value at PINB#j and checks if 0 (pressed)
					{
						int number = (KEYCHAR[i][j]); // Set number to the corresponding button pressed
						timer0(number); // Call the timer function to calculate Duty Cycle from specified number
					}
				}
				PORTD |= (1 << (4+i)); // Set row (D4-D7) back to 1 (HIGH) to repeat the process
			}
}
 
int main(void)
{
	cli(); // Clears all global interrupt flag to prevent interrupts
	uart_init();
	sei(); // Enables global interrupts
	// Timer2 Setup
	
	 	TCCR2A |= (1 << WGM21); // Set Timer2 to CTC used to generate interrupt and scans the keypad and toggles GPIO pin
	 	TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20); 	//Set the pre-scaler to 1024: 16.384 ms
	 	OCR2A = 92; // Set timer interrupt period from Red ID: 821432(5)15 = 6 ms => 93 => 92
		TIMSK2 |= (1 << OCIE2A); // Sets ISR CompA for vector by setting Timer Interrupt Mask Register for Timer 2 at bit 1	
	
	while(1)
	{
	
	}
}

// ISR will save the program state before the interrupt is made then call it and run whatever is inside the function. After, it restores the program state and resumes it.

ISR(TIMER2_COMPA_vect) 
{
	keypad_scan(); // Scans the keypad when interrupt happens (6 ms)
}

ISR(TIMER0_COMPA_vect) // Timer0 interrupt compare A register (1.6 ms)
{
	PORTB |= (1 << 5); // Turns on LED 
}

ISR(TIMER0_COMPB_vect) // Timer0 interrupt compare B register (Duty Cycle)
{
	PORTB &= ~(1 << 5); // Turns off the LED at duty cycle percentage
	if (OCR0B == 0)
	{
		DDRB &= ~(1 << 5); // This fully turns off the LED
	}
	else
	{
		DDRB |= (1 << 5); // If not 0, then LED remains as an output.
	}
}