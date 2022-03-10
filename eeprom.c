/*
 * Author : Lawrence

For this lab assignment, you must program the EEPROM unit of your AVR device dynamically
during run time. The functionality of your code should be as follows:
� You should download your code to the device only once!
� If the program runs for the first time, your code should set up a PWM waveform with 0%
duty cycle.
� Whenever your AVR decide is unplugged and plugged back in, the duty cycle the
waveform should be increased by 10%.
� After 100%, the duty cycle should cycle back to 0%.
For example, the following is a sample sequence (Remember that the code is downloaded to the
device only once. Every �next time� below means the device is unplugged and plugged back in):
� If the AVR is running the code for the first time, the duty cycle % of the waveform is 0.
� If the AVR is running the code for the next time, the duty cycle % of the waveform is 10.
� If the AVR is running the code for the next time, the duty cycle % of the waveform is 20.
� �
� If the AVR is running the code for the next time, the duty cycle % of the waveform is 90.
� If the AVR is running the code for the next time, the duty cycle % of the waveform is 100.
� If the AVR is running the code for the next time, the duty cycle % of the waveform is 0.
� �
To demonstrate the duty cycle of the waveform, you should use a timer with 5ms period and
demonstrate the PWM waveform on the on-board LED (remember Lab 05). Successful
demonstration of your implementation includes finding the parameters to setup this timer too.
-----------------------------------------------------------------------------------------------------
Checkpoint for the lab:
� Demonstrate which address location in EEPROM you can read/write for the
purposes of this lab, and then explain if this location matters.

Location matters as you only have so many memory address that the EEPROM contains.
The maximum bytes is 1K which is 1024.

� Explain if there is a default value written in the EEPROM location you
mentioned previously.
� Demonstrate the default value written in the EEPROM location by sending it
via USART to the Data Visualizer Terminal.
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#define BAUD 9600
#define BAUDRATE ((F_CPU) / (16UL * BAUD)-1)

uint8_t ByteofData;

void uart_transmit (char buffer)
{
	while (!( UCSR0A & (1<<UDRE0))); // Wait until buffer is empty (character reaches the terminal) before sending next character. While UDRE0 = 1, it is empty.

	UDR0 = buffer; // Set load data register to holder
	_delay_ms(200);
}

void uart_init ()
{
	/* UBRR IS 12-BITS  */
	UBRR0H = (BAUDRATE>>8); // Shift by 8 to the right clear it and to set HIGH BIT REGISTER to 0
	UBRR0L = (BAUDRATE); // Set low bit register to the calculated BAUDRATE
	
	UCSR0C |= (1<<UCSZ00) | (1<<UCSZ01); // Setting up 8-bit data frame and stop bit in USCRC register. Stop bit (USBSn) is initially 0, so no change
	UCSR0B |= (1<<TXEN0) | (1<<RXEN0); // Enable transmitter & receiver in UCSRB register
	
	/*TIMER SETUP*/
	DDRB |= (1 << 5); // Make LED an output
	TCCR0A |= (1 << WGM01); // Set timer to CTC
	OCR0A = 77.125; // Set max value to 5 ms
	TCCR0B |= (1 << CS00) | (1 << CS02); // Sets pre-scaler of 1024 and starts the timer 
}

// We have 1024 memory locations we access (0-1023 addresses)
// Each default value in the memory location is defaulted to 0xFF (255) which is the max range of a uint8_t data type (char)


void read_eeprom()
{
	ByteofData = eeprom_read_byte((uint8_t*) 30); // Set ByteofData to the EEPROM address location at 30
}

void write_eeprom()
{
	eeprom_update_byte((uint8_t*) 30, ByteofData); // Update location 30 to ByteofData
}

int main(void)
{
	uart_init();
	read_eeprom(); // First read and then set the value inside the memory location 30 to ByteofData
	double x = ByteofData; // Example: x = 3.0
	x = x/10.0; // Now x = 0.3 
	
	ByteofData = ByteofData + 1; // Increment the read ByteofData by 1 to represent an increase in duty cycle;
	
	if (ByteofData > 10) // If ByteofData = 11, reset it to 0
	{
		ByteofData = 0;
	}
	
	write_eeprom(); // Write the incremented ByteofData to memory location 30
	
    while (1) 
    {		
		OCR0B = (x * OCR0A); // This will set OCR0B value to the correctly duty cycle percentage 
			if (OCR0B == 0)
			{
				PORTB &= ~(1 << 5);
			}
			else
			{
				PORTB |= (1 << 5); // Turn on LED
				while ( (TIFR0 & (1 << OCF0B)) == 0){} // Wait for the OCR0B overflow event, (when counter reaches OCR0B value) 
				TIFR0 |= (1 << OCF0B); // Reset the overflow flag
				PORTB &= ~(1 << 5); // Turn off LED
				while ( (TIFR0 & (1 << OCF0A)) == 0){} // Wait for OCR0A overflow event, (when counter reaches OCR0A value)
				TIFR0 |= (1 << OCF0A); // Reset the overflow flag
			}
			 		  
    }
}


/* THIS IMPLEMENTATION IS NOT NECESSARY ANYMORE. THANK YOU ANUP FOR POINTING THIS OUT.
			  switch (x)
			  {
				  case 0:
// 					OCR0B = 0; // Set OCR0B to 0% duty cycle
// 					
// 					PORTB |= (1 << 5); // Turn on LED
// 					while ( (TIFR0 & (1 << OCF0B)) == 0){} // Wait for the OCR0B overflow event, (when counter reaches OCR0B value) 
// 					TIFR0 |= (1 << OCF0B); // Reset the overflow flag
// 					PORTB &= ~(1 << 5); // Turn off LED
// 					while ( (TIFR0 & (1 << OCF0A)) == 0){} // Wait for OCR0A overflow event, (when counter reaches OCR0A value)
// 					TIFR0 |= (1 << OCF0A); // Reset the overflow flag
						
					PORTB &= ~(1 << 5); // Turn off LED					
					break;
					
				 case 1:
					OCR0B = 7; // Set OCR0B to 10% duty cycle
					
					PORTB |= (1 << 5); // Turn on LED
					while ( (TIFR0 & (1 << OCF0B)) == 0){} // Wait for the OCR0B overflow event, (when counter reaches OCR0B value) 
					TIFR0 |= (1 << OCF0B); // Reset the overflow flag
					PORTB &= ~(1 << 5); // Turn off LED
					while ( (TIFR0 & (1 << OCF0A)) == 0){} // Wait for OCR0A overflow event, (when counter reaches OCR0A value)
					TIFR0 |= (1 << OCF0A); // Reset the overflow flag
					
					break;
				  case 2:
					OCR0B = 15; // Set OCR0B to 20% duty cycle
					
					PORTB |= (1 << 5); // Turn on LED
					while ( (TIFR0 & (1 << OCF0B)) == 0){} // Wait for the OCR0B overflow event, (when counter reaches OCR0B value) 
					TIFR0 |= (1 << OCF0B); // Reset the overflow flag
					PORTB &= ~(1 << 5); // Turn off LED
					while ( (TIFR0 & (1 << OCF0A)) == 0){} // Wait for OCR0A overflow event, (when counter reaches OCR0A value)
					TIFR0 |= (1 << OCF0A); // Reset the overflow flag
					
					break;
				  case 3:
				  	OCR0B = 23; // Set OCR0B to 30% duty cycle
					
					PORTB |= (1 << 5); // Turn on LED
					while ( (TIFR0 & (1 << OCF0B)) == 0){} // Wait for the OCR0B overflow event, (when counter reaches OCR0B value) 
					TIFR0 |= (1 << OCF0B); // Reset the overflow flag
					PORTB &= ~(1 << 5); // Turn off LED
					while ( (TIFR0 & (1 << OCF0A)) == 0){} // Wait for OCR0A overflow event, (when counter reaches OCR0A value)
					TIFR0 |= (1 << OCF0A); // Reset the overflow flag
					
					break;
				  case 4:
					OCR0B = 30; // Set OCR0B to 10% duty cycle
					
					PORTB |= (1 << 5); // Turn on LED
					while ( (TIFR0 & (1 << OCF0B)) == 0){} // Wait for the OCR0B overflow event, (when counter reaches OCR0B value) 
					TIFR0 |= (1 << OCF0B); // Reset the overflow flag
					PORTB &= ~(1 << 5); // Turn off LED
					while ( (TIFR0 & (1 << OCF0A)) == 0){} // Wait for OCR0A overflow event, (when counter reaches OCR0A value)
					TIFR0 |= (1 << OCF0A); // Reset the overflow flag
					
					break;
				  case 5:
					OCR0B = 38; // Set OCR0B to 50% duty cycle
					
					PORTB |= (1 << 5); // Turn on LED
					while ( (TIFR0 & (1 << OCF0B)) == 0){} // Wait for the OCR0B overflow event, (when counter reaches OCR0B value) 
					TIFR0 |= (1 << OCF0B); // Reset the overflow flag
					PORTB &= ~(1 << 5); // Turn off LED
					while ( (TIFR0 & (1 << OCF0A)) == 0){} // Wait for OCR0A overflow event, (when counter reaches OCR0A value)
					TIFR0 |= (1 << OCF0A); // Reset the overflow flag
					
					break;
				  case 6:
					OCR0B = 46; // Set OCR0B to 60% duty cycle
					
					PORTB |= (1 << 5); // Turn on LED
					while ( (TIFR0 & (1 << OCF0B)) == 0){} // Wait for the OCR0B overflow event, (when counter reaches OCR0B value) 
					TIFR0 |= (1 << OCF0B); // Reset the overflow flag
					PORTB &= ~(1 << 5); // Turn off LED
					while ( (TIFR0 & (1 << OCF0A)) == 0){} // Wait for OCR0A overflow event, (when counter reaches OCR0A value)
					TIFR0 |= (1 << OCF0A); // Reset the overflow flag
					
					break;
				  case 7:
				  	OCR0B = 54; // Set OCR0B to 70% duty cycle
					
					PORTB |= (1 << 5); // Turn on LED
					while ( (TIFR0 & (1 << OCF0B)) == 0){} // Wait for the OCR0B overflow event, (when counter reaches OCR0B value) 
					TIFR0 |= (1 << OCF0B); // Reset the overflow flag
					PORTB &= ~(1 << 5); // Turn off LED
					while ( (TIFR0 & (1 << OCF0A)) == 0){} // Wait for OCR0A overflow event, (when counter reaches OCR0A value)
					TIFR0 |= (1 << OCF0A); // Reset the overflow flag
					
					break;
				  case 8:
				  	OCR0B = 61; // Set OCR0B to 80% duty cycle
					
					PORTB |= (1 << 5); // Turn on LED
					while ( (TIFR0 & (1 << OCF0B)) == 0){} // Wait for the OCR0B overflow event, (when counter reaches OCR0B value) 
					TIFR0 |= (1 << OCF0B); // Reset the overflow flag
					PORTB &= ~(1 << 5); // Turn off LED
					while ( (TIFR0 & (1 << OCF0A)) == 0){} // Wait for OCR0A overflow event, (when counter reaches OCR0A value)
					TIFR0 |= (1 << OCF0A); // Reset the overflow flag
					
					break;
				  case 9:
				  	OCR0B = 69; // Set OCR0B to 90% duty cycle
					
					PORTB |= (1 << 5); // Turn on LED
					while ( (TIFR0 & (1 << OCF0B)) == 0){} // Wait for the OCR0B overflow event, (when counter reaches OCR0B value) 
					TIFR0 |= (1 << OCF0B); // Reset the overflow flag
					PORTB &= ~(1 << 5); // Turn off LED
					while ( (TIFR0 & (1 << OCF0A)) == 0){} // Wait for OCR0A overflow event, (when counter reaches OCR0A value)
					TIFR0 |= (1 << OCF0A); // Reset the overflow flag
					
					break;
				  case 10:
				  	OCR0B = 77; // Set OCR0B to 100% duty cycle
					
					PORTB |= (1 << 5); // Turn on LED
					while ( (TIFR0 & (1 << OCF0B)) == 0){} // Wait for the OCR0B overflow event, (when counter reaches OCR0B value) 
					TIFR0 |= (1 << OCF0B); // Reset the overflow flag
					PORTB &= ~(1 << 5); // Turn off LED
					while ( (TIFR0 & (1 << OCF0A)) == 0){} // Wait for OCR0A overflow event, (when counter reaches OCR0A value)
					TIFR0 |= (1 << OCF0A); // Reset the overflow flag
					
					break;
					
				  case 11:
// 					OCR0B = 0; // Set OCR0B to 0% duty cycle
// 					
// 					PORTB |= (1 << 5); // Turn on LED
// 					while ( (TIFR0 & (1 << OCF0B)) == 0){} // Wait for the OCR0B overflow event, (when counter reaches OCR0B value) 
// 					TIFR0 |= (1 << OCF0B); // Reset the overflow flag
// 					PORTB &= ~(1 << 5); // Turn off LED
// 					while ( (TIFR0 & (1 << OCF0A)) == 0){} // Wait for OCR0A overflow event, (when counter reaches OCR0A value)
// 					TIFR0 |= (1 << OCF0A); // Reset the overflow flag
					
					PORTB &= ~(1 << 5); // Turn off LED
					
					ByteofData = 0; // Set ByteofData to 0
					write_eeprom(); // Write ByteofData into memory location
					break;
					
				  default:
					break;
			  }
	*/







/*
PLACE THIS INSIDE WHILE(1) TO TRANSMIT THE DATA (USED FOR CHECKPOINT)
char result[10];
sprintf (result, "# = %d ", ByteofData);
for (int x = 0; x < strlen(result); x++)
{
	uart_transmit(result[x]);
}
*/