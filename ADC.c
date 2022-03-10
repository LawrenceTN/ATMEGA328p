/*
 * Author : Lawrence
 */ 

#define F_CPU 16000000UL
#define BAUD 9600
#define BAUDRATE ((F_CPU) / (16UL * BAUD)-1)
#include <avr/io.h>
#include <avr/interrupt.h>


void adc_init() 
{
	DDRB |= (1 << 5); // Set LED0 as an output
	// Red ID# 82132515 = 821432XYZ
	DDRC &= ~(1 << PINC1); // Setting output of potentiometer to input channel 1 (PC1)  (Y = 1%8 = 1)
	ADMUX |= (1 << REFS0) | (1 << MUX0); // Sets the reference voltage: VCC reference - AVcc with external capacitor at AREF pin (REFS0). Analog Input Channel 0 is set to 1 
	ADCSRA |= (1 << ADEN) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Enable the ADC auto trigger, Enable the AD Interrupt (ADC Conversion Complete Interrupt), and pre-scaler 128 
	ADCSRB |= (1 << ADTS0) | (1 << ADTS1); // Enable compare match interrupt for Match B
}

void timer_init() 
{
	// TIMER0: Generates the PWM waveform to control LED brightness from the A/D
	// PWM Frequency = 821432515 = 821432XYZ = (5+1) = 6*100 = 600 Hz
	TCCR0A |= (1 << WGM01); // Set Timer0 to CTC mode used for LED waveform
	TCCR0B |= (1 << CS02); // Set the pre-scaler to 256
	OCR0A = 103; // PWM frequency = 600Hz -> 1/600 = 1.667ms -> 104 -> 103
	TIMSK0 |= (1 << OCIE0A) | (1 << OCIE0B); // Sets ISR CompA and ISR compB vector by setting 1 to Timer Interrupt Mask Register for Timer 0 at bit 1 and 2
	
	// TIMER2: TIMER2 will generate an interrupt every 6 ms and will start the ADC conversion process 
	TCCR2A |= (1 << WGM21); // Set Timer2 to CTC used to generate interrupt and start the A/D Conversion and toggles the GPIO pin
	TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20); 	//Set the pre-scaler to 1024: 16.384 ms
	OCR2A = 92; // Set timer interrupt period from Red ID: 821432(5)15 = 5+1 = 6 ms => 93 => 92
	TIMSK2 |= (1 << OCIE2A); // Sets ISR CompA for vector by setting Timer Interrupt Mask Register for Timer 2 at bit 1
	// No need for an auto-trigger since the conversion is in Timer2
}	

int main(void)
{ 
	adc_init(); // Set up the ADC initialization
	timer_init(); // Set up timer interrupts initialization
	sei(); // Enable global interrupts
	while (1) {}
}

ISR (TIMER0_COMPA_vect) // Timer0 Interrupt for Compare A
{
	if (OCR0B > 0)
	{
		PORTB |= (1 << 5);// Turn on the LED for PWM
	}
}

ISR (TIMER0_COMPB_vect) // Timer0 Interrupt for Compare B
{
	PORTB &= ~(1 << 5); // Turn off the LED for PWM
}

ISR (TIMER2_COMPA_vect) // Timer2 Interrupt for the conversion
{
	ADCSRA |= (1 << ADSC); // Sets ADC Status Register A: Sets 1 to Start Conversion Bit to start the conversion  
}

ISR (ADC_vect) // When the conversion is finished, trigger this interrupt
{
	double variable = ADC; 
	OCR0B = (variable * OCR0A) / 1024.0; // Sets the OCR0B value based on the ADC variable that will adjust the PWM Duty Cycle
}