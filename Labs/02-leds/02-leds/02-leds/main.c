/***********************************************************************
 * 
 * Alternately toggle two LEDs when a push button is pressed.
 * ATmega328P (Arduino Uno), 16 MHz, AVR 8-bit Toolchain 3.6.2
 *
 * Copyright (c) 2018-Present Tomas Fryza
 * Dept. of Radio Electronics, Brno University of Technology, Czechia
 * This work is licensed under the terms of the MIT license.
 * 
 **********************************************************************/

/* Defines -----------------------------------------------------------*/
#define LED_GREEN   PB5     // AVR pin where green LED is connected
#define LED_SECOND	PC0		// AVR pin where red LED is connected
#define BUTTON		PD7		// AVR pin where button is connected
#define BLINK_DELAY 500
#ifndef F_CPU
# define F_CPU 16000000     // CPU frequency in Hz required for delay
#endif

/* Includes ----------------------------------------------------------*/
#include <util/delay.h>     // Functions for busy-wait delay loops
#include <avr/io.h>         // AVR device-specific IO definitions

/* Functions ---------------------------------------------------------*/
/**********************************************************************
 * Function: Main function where the program execution begins
 * Purpose:  Toggle two LEDs when a push button is pressed.
 * Returns:  none
 **********************************************************************/
int main(void)
{
    // Green LED at port B
    // Set pin as output in Data Direction Register...
    DDRB = DDRB | (1<<LED_GREEN);
    // ...and turn LED off in Data Register
    PORTB = PORTB & ~(1<<LED_GREEN);

    // Configure the second LED at port C
	DDRC |= (1<<LED_SECOND);
	PORTC &= ~(1<<LED_SECOND);

    // Configure Push button at port D and enable internal pull-up resistor

	DDRD &= ~(0<<BUTTON);
	PORTD |= (1<<BUTTON);

    // Infinite loop
    while (1)
    {
        _delay_ms(BLINK_DELAY);
		PORTC ^= (1<<LED_SECOND);
		PORTB ^= (1<<LED_GREEN);
		_delay_ms(BLINK_DELAY);
		PORTC ^= (1<<LED_SECOND);
		PORTB ^= (1<<LED_GREEN);
    }

    // Will never reach this
    return 0;
}