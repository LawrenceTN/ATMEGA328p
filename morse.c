/*
 * Author : Lawrence Nieto
 */ 

/*
Make your Name and Red ID blink in Morse Code on the AVR Xplained mini 328 board.
The method you choose must output any standard null-terminated string containing characters A-Z or 0-9 in Morse code on the board's LED. 
For the demo, a child function, called from main( ) should output your name and Red ID number in Morse code, using a method that will work for any arbitrary string constant (in other words, main should pass the address of the null terminated ASCII string to the child function)
The program is recommended to have different functions for dot, dash, and spacing. (The functions can be called based on the desired output.) 
Using functions make it simple to design multiple outputs, as you simply call on what you need whenever.
Assume, one unit of time for this program will be 200ms. 

A dot is equivalent to one unit (200ms)
A dash is equivalent to three units (600ms)
The space between the dot/dash units of the same letter is one unit (200ms) 
The space between different letters is three units (600ms) 
The space between words is seven units (1400ms)

The program should treat your input string as one word, and put a longer space between each run of the initials. The program is intended to run on an endless loop.

MORSE CODE PICTURE:
https://upload.wikimedia.org/wikipedia/commons/thumb/b/b5/International_Morse_Code.svg/1200px-International_Morse_Code.svg.png
*/

#define F_CPU 16000000UL  // 16MHz clock from the debug processor
#include <avr/io.h>
#include <util/delay.h>


void dot()
{
	PORTB |= (1<<PORTB5); // LED ON
	_delay_ms(200); // DELAY 
	PORTB &= ~(1<<PORTB5); // LED OFF
	
	_delay_ms(200); // DELAY BETWEEN DOTS/DASH/SPACE
	
	return;
}

void dash()
{
	PORTB |= (1<<PORTB5); // LED ON
	_delay_ms(600); // DELAY
	PORTB &= ~(1<<PORTB5); // LED OFF
	
	_delay_ms(200); // DELAY BETWEEN DOTS/DASH/SPACE
	
	return;
}

void spc()
{
	_delay_ms(800); // DELAY OF SPACE (1400 - (200+400) from between letters/numbers/dashes/dots = 800)
	return;
}

void Morse(char nameID[])
{

	for (int i = 0; i <= 23; i++) // The length of my array (full name and ID)
	{
		if (nameID[i] > 47 && nameID[i] < 58) // Check if nameID[i] is a number
		{
			int x = nameID[i] - '0'; // Translates the char '#' to an int '#'
			int value = (x % 5); // Set value to the (nameID[i] modulo 5) to set the corresponding pattern of dots/dashes

			if (nameID[i] <= 53) // if nameID[i] is a number from 0-5
			{
				for(i = 0; i < value; i++) // First for loop to produce the correct number of dots
				{
					dot();
				}
				for(i = 0; i < (5 - value); i++) // Second for loop to produce toe correct number of dashes.
				{
					dash();
				}
			}
			else // if nameID[i] is a number from 6-9
			{
				for(i = 0; i < value; i++) // Ex: 8 mod 5 = 3. 3 dashes, 2 dots.
				{
					dash(); 
				}
				for(i = 0; i < (5 - value); i++)
				{
					dot();
				}
			}
		}

		else 
		{		
			switch (nameID[i])
			{
				case 'L':
					dot(); dash(); dot(); dot();
					_delay_ms(400); // Delay in-between letters. Only 400 because 200 ms comes from the end of dot/dash = 600.
					break;
				case 'A':
					dot(); dash();
					_delay_ms(400);
					break;				
				case 'W':
					dot(); dash(); dash();
					_delay_ms(400);
					break;
				case 'R':
					dot(); dash(); dot();
					_delay_ms(400); 
					break;
				case 'E':
					dot();
					_delay_ms(400); 
					break;
				case 'N':
					dash(); dot();
					_delay_ms(400); 
					break;
				case 'C':
					dash(); dot(); dash(); dot();
					_delay_ms(400);
					break;
				case 'I':
					dot(); dot();
					_delay_ms(400); 
					break;
				case 'T':
					dash();
					_delay_ms(400); 
					break;
				// Rest of the alphabet (letters not used in my name);
								case 'B':
									dash(); dot(); dot(); dot();
									_delay_ms(400);
									break;
								case 'D':
									dash(); dot(); dot();
									_delay_ms(400);
									break;
								case 'F':
									dot(); dot(); dash(); dot();
									_delay_ms(400);
									break;
								case 'G':
									dash(); dash(); dot();
									_delay_ms(400);
									break;
								case 'H':
									dot(); dot(); dot(); dot();
									_delay_ms(400);
									break;
								case 'J':
									dot(); dash(); dash(); dash();
									_delay_ms(400);
									break;
								case 'K':
									dash(); dot(); dash();
									_delay_ms(400);
									break;
								case 'M':
									dash(); dash();
									_delay_ms(400);
									break;
								case 'P':
									dot(); dash(); dash(); dot();
									_delay_ms(400);
									break;
								case 'Q':
									dash(); dash(); dot(); dash();
									_delay_ms(400);
									break;
								case 'S':
									dot(); dot(); dot(); 
									_delay_ms(400);
									break;
								case 'U':
									dot(); dot(); dash();
									_delay_ms(400);
									break;
								case 'V':
									dot(); dot(); dot(); dash();
									_delay_ms(400);
									break;
								case 'X':
									dash(); dot(); dot(); dash();
									_delay_ms(400);
									break;
								case 'Y':
									dash(); dot(); dash(); dash();
									_delay_ms(400);
									break;
								case 'Z':
									dash(); dash(); dot(); dot();
									_delay_ms(400);
									break;
				case ' ':
					spc(); 
				default:
					break;
			}
		}
	}
		return;

}

int main(void)
{
	char nameID[] = "LAWRENCE NIETO 821432515"; // Array size 23
	while(1)
	{
		DDRB |= (1<<DDB5); // Sets Data Direction Register at Port 'B5' to an output (1) to allow LED to turn on/off.
		Morse(nameID); // Function call with nameID array as parameter.
	}
	return 0;
}