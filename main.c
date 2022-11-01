/***
* SNES_JR_IGR - ATtiny Firmware
* By: Viper33802
* Source: https://github.com/viper33802/SNES_JR_IGR
* License: GPLv3
* Desc: Sniff's SNES controller port for key-combo,
*	resets SNES by pulling PIN_RESET high. (5v)
*   Also Adds a Power RGB LED.
* Original Project By: Nold
* Original Source: https://github.com/Nold360/lazy_mans_snes_reset
*/#include <avr/io.h>
#include <avr/interrupt.h>

#ifndef F_CPU
#define F_CPU 9600000UL
#endif

#include <util/delay.h>
#include "ws2812b_attiny13.h"

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

// Define a list of colors
const struct color {
	const uint8_t red;
	const uint8_t green;
	const uint8_t blue;
	} COLORS[] = {
//	{0xC0, 0xC0, 0xC0}, // silver
	{0x80, 0x80, 0x80}, // gray
	{0xFF, 0xFF, 0xFF}, // white
//	{0x80, 0x00, 0x00}, // maroon
	{0xFF, 0x00, 0x00}, // red
	{0x80, 0x0D, 0x80}, // purple
//	{0xFF, 0x00, 0xFF}, // fuchsia
	{0x00, 0x80, 0x00}, // green
//	{0x00, 0xFF, 0x00}, // lime
//	{0x80, 0x80, 0x00}, // olive
	{0xFF, 0xFF, 0x00}, // yellow
//	{0x00, 0x00, 0x80}, // navy
	{0x00, 0x00, 0xFF}, // blue
//	{0x00, 0x80, 0x80}, // teal
	{0x00, 0xFF, 0xFF}, // aqua
};

// Button combination for reset
uint16_t const reset_mask = ((1 << START) | (1 << SELECT) | (1 << BUTTON_L));
uint16_t const restart_mask = ((1 << START) | (1 << SELECT) | (1 << BUTTON_R));
uint16_t const reset_enable = ((1 << START) | (1 << SELECT) | (1 << BUTTON_Y));
uint16_t const restart_enable = ((1 << START) | (1 << SELECT) | (1 << BUTTON_X));
uint16_t const led_enable = ((1 << SELECT) | (1 << BUTTON_L) | (1 << BUTTON_Y));
uint16_t const led_color = ((1 << SELECT) | (1 << BUTTON_R) | (1 << BUTTON_Y));

//EEPROM Addresses for Settings
const unsigned char resetaddr = 00;
const unsigned char restartaddr = 01;
const unsigned char ledaddr = 02;
const unsigned char ledcaddr = 03;
const uint8_t ledpin = _BV(PB3);

volatile unsigned char rset;
volatile unsigned char rsrt;
unsigned char led;
unsigned char ledc;
int lc;

void EEPROM_write(unsigned char ucAddress, unsigned char ucData)
{
	/* Wait for completion of previous write */
	while(EECR & (1<<EEPE))
	;
	/* Set Programming mode */
	EECR = (0<<EEPM1)|(0>>EEPM0);
	/* Set up address and data registers */
	EEARL = ucAddress;
	EEDR = ucData;
	/* Write logical one to EEMPE */
	EECR |= (1<<EEMPE);
	/* Start eeprom write by setting EEPE */
	EECR |= (1<<EEPE);
}

unsigned char EEPROM_read(unsigned char ucAddress)
{
	/* Wait for completion of previous write */
	while(EECR & (1<<EEPE))
	;
	/* Set up address register */
	EEARL = ucAddress;
	/* Start eeprom read by writing EERE */
	EECR |= (1<<EERE);
	/* Return data from data register */
	return EEDR;
}

void Update_Led()
{
	unsigned char k = EEPROM_read(ledcaddr);
	unsigned char v = EEPROM_read(ledaddr);
	int lcolor = (int)k;
	if (v == 0) {
		struct color color = COLORS[lcolor];
		ws2812b_set_color(ledpin, color.red, color.green, color.blue);
	}
	else {
		ws2812b_set_color(ledpin, 0x00, 0x00, 0x00);
	}
}

//Retreive all settings from EEPROM
void Update_Settings()
{
	rset = EEPROM_read(resetaddr);
	rsrt = EEPROM_read(restartaddr);
	led = EEPROM_read(ledaddr);
	ledc = EEPROM_read(ledcaddr);
}

// Toggle EEPROM Bit
void FlipEPBit(unsigned char EPaddr)
{
	unsigned char w = EEPROM_read(EPaddr);
	w =! w;
	EEPROM_write(EPaddr, w);
	Update_Settings();
	_delay_ms(250);
}

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

	//Toggle Reset
	if((button_state ^ reset_mask) == 0 && rset == 0) {
		PORTB |=  (1 << PIN_RESET);
		_delay_ms(200);
		PORTB &= ~(1 << PIN_RESET);
	}
	
	//Toggle Restart
	if((button_state ^ restart_mask) == 0 && rsrt == 0) {
		PORTB |=  (1 << PIN_RESET);
		_delay_ms(1500);
		PORTB &= ~(1 << PIN_RESET);
	}
	
	//Enable Disable Reset
	if((button_state ^ reset_enable) == 0) {
		FlipEPBit(resetaddr);
		Update_Settings();
	}

	//Enable Disable Restart
	if((button_state ^ restart_enable) == 0) {
		FlipEPBit(restartaddr);
		Update_Settings();
	}
	
	//Enable Disable LED
	if((button_state ^ led_enable) == 0) {
		FlipEPBit(ledaddr);
		Update_Settings();
	}

	//Cycle LED Colors
	if((button_state ^ led_color) == 0) {
		ledc = EEPROM_read(ledcaddr);
		lc = (int)ledc;
		lc++;
		ledc = (unsigned char)lc;
		if (lc > 8) {
			ledc = 0x01;
			lc = (int)ledc;
		}
		EEPROM_write(ledcaddr, ledc);
		Update_Settings();
		_delay_ms(250);
	}

	sei();
}

int main (void)
{
	// Set PIN_RESET as output, all others are inputs
	DDRB |= (1 << PIN_RESET);
	
	// Get current EEPROM settings
	Update_Settings();

	//Enable Interrupt INT0, at falling edge (PIN_LATCH)
	GIMSK = (1<<INT0);
	MCUCR = (1<<ISC01);

	Update_Led();

	sei();

	//Do nothing, only on external interrupt
	while(1){
		_delay_us(1);
	}
}