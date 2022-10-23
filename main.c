/***
* Lazy Man's SNES Reset - ATtiny Firmware
* By: Nold
* Modified By: Viper33802
* License: GPLv3
* Desc: Sniff's SNES controller port for key-combo, then resets
*       SNES by pulling PIN_RESET high. (5v)
* Source: https://github.com/viper33802/lazy_mans_snes_reset
*/

#include <avr/io.h>
#include <avr/interrupt.h>

#ifndef F_CPU
	#define F_CPU 9600000UL
#endif

#include <util/delay.h>

//Pin Configuration
#define PIN_CLOCK PB0
#define PIN_LATCH PB1 // <-- has to be an interrupt pin!
#define PIN_DATA  PB2
#define PIN_RESET PB4

//SNES Buttons:
#define BUTTON_A 	8
#define BUTTON_B 	0
#define BUTTON_X	9
#define BUTTON_Y 	1
#define BUTTON_L   10
#define BUTTON_R   11
#define DPAD_UP 	4
#define DPAD_DOWN 	5
#define DPAD_LEFT 	6
#define DPAD_RIGHT 	7
#define START 		3
#define SELECT 		2

// Button combination for reset and restart
uint16_t const reset_mask = ((1 << START) | (1 << BUTTON_L) | (1 << SELECT));
uint16_t const restart_mask = ((1 << START) | (1 << BUTTON_R) | (1 << SELECT));

// Interrupt callback, which reads the controller data
ISR(INT0_vect)
{
	cli();
	uint8_t i;
	uint16_t button_state = 0;

	//read first 12bit, ignore last 4
	for(i = 0; i<12; i++)
	{
		//Read data on falling edge
		uint8_t clk = (PINB & (1 << PIN_CLOCK));
		while((PINB & (1 << PIN_CLOCK)) == clk)
			_delay_us(1);

		// Is DATA == LOW, shift 1 into bitmap
		if ((PINB & (1 << PIN_DATA)) == 0)
			button_state |= (1 << i);

		// Wait for next clock cycle
		clk = (PINB & (1 << PIN_CLOCK));
		while((PINB & (1 << PIN_CLOCK)) == clk)
			_delay_us(1);
	}

	//Toggle Reset Game
	if((button_state ^ reset_mask) == 0) {
		PORTB |=  (1 << PIN_RESET);
		_delay_ms(100);
		PORTB &= ~(1 << PIN_RESET);
		
	//Toggle Restart Console
	if((button_state ^ restart_mask) == 0) {
		PORTB |=  (1 << PIN_RESET);
		_delay_ms(2000);
		PORTB &= ~(1 << PIN_RESET);
	}

	sei();
}

int main (void)
{
 	// Set PIN_RESET as output, all others are inputs
 	DDRB |= (1 << PIN_RESET);

	//Enable Interrupt INT0, at falling edge (PIN_LATCH)
	GIMSK = (1<<INT0);
	MCUCR = (1<<ISC01);

	sei();

	//Do nothing, only on external interrupt
	while(1){
		_delay_us(1);
	}
}


