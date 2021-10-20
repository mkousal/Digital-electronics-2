/***********************************************************************
 * 
 * Seven-segment display library for AVR-GCC.
 * ATmega328P (Arduino Uno), 16 MHz, AVR 8-bit Toolchain 3.6.2
 *
 * Copyright (c) 2019-Present Tomas Fryza
 * Dept. of Radio Electronics, Brno University of Technology, Czechia
 * This work is licensed under the terms of the MIT license.
 *
 **********************************************************************/

/* Includes ----------------------------------------------------------*/
#ifndef F_CPU
# define F_CPU 16000000
#endif
#include <util/delay.h>
#include "gpio.h"
#include "segment.h"

uint8_t segment_value[] = {
	// abcdefgDP
	0b00000011,	// 0
	0b10011111,	// 1
	0b00100101,	// 2
	0b00001101,	// 3
	0b10011001,	// 4
	0b01001001,	// 5
	0b01000001,	// 6
	0b00011111, // 7
	0b00000001,	// 8
	0b00001001	// 9
};

uint8_t segment_position[] = {
	0b00010000,	// position 0
	0b00100000,	// position 1
	0b01000000,	// position 2
	0b10000000	// position 3
};

uint8_t segment_snake_value[] = {
	0b01111111,	// a
	0b10111111,	// b
	0b11011111,	// c
	0b11101111,	// d
	0b11110111,	// e
	0b11111011	// f
};

/* Function definitions ----------------------------------------------*/
/**********************************************************************
 * Function: SEG_init()
 * Purpose:  Configure SSD signals LATCH, CLK, and DATA as output.
 * Returns:  none
 **********************************************************************/
void SEG_init(void)
{
    /* Configuration of SSD signals */
    GPIO_config_output(&DDRD, SEG_LATCH);
    GPIO_config_output(&DDRD, SEG_CLK);
    GPIO_config_output(&DDRB, SEG_DATA);
}

/**********************************************************************
 * Function: SEG_update_shift_regs()
 * Purpose:  Display number at one position of the SSD.
 * Input:    number - Number to be displayed (0,1,2,...., active low)
 *           position - Position of the display where the numbers are to 
 *                      be displayed (p3 p2 p1 p0 xxxx, active high)
 *			 DP - Decimal point displayed (0 - off), default OFF
 * Returns:  none
 **********************************************************************/
void SEG_update_shift_regs(uint8_t number, uint8_t position, uint8_t dp)
{
    uint8_t bit_number;
	number = segment_value[number];
	if (dp != 0)
		number &= ~(0x01);
	position = segment_position[position];

    // Pull LATCH, CLK, and DATA low
	GPIO_write_low(&PORTD, SEG_LATCH);
	GPIO_write_low(&PORTD, SEG_CLK);
	GPIO_write_low(&PORTB, SEG_DATA);

    // Wait 1 us
	_delay_us(1);

    // Loop through the 1st byte (number)
    // a b c d e f g DP (active low values)
    for (bit_number = 0; bit_number < 8; bit_number++)
    {
        // Test LSB of "number" by & (faster) or % (slower) and... 
        // ...output DATA value
		if ((number & 0x01) == 1)
			GPIO_write_high(&PORTB, SEG_DATA);
		else
			GPIO_write_low(&PORTB, SEG_DATA);

		SEG_clk_2us();
        // Shift "number"
        number = number >> 1;
    }

    // Loop through the 2nd byte (position)
    // p3 p2 p1 p0 . . . . (active high values)
    for (bit_number = 0; bit_number < 8; bit_number++)
    {
        // Test LSB of "position" by & (faster) or % (slower) and... 
        // ...output DATA value
		if ((position & 0x01) == 1)
			GPIO_write_high(&PORTB, SEG_DATA);
		else
			GPIO_write_low(&PORTB, SEG_DATA);

		SEG_clk_2us();
        // Shift "position"
        position = position >> 1;
    }

    // Pull LATCH high
	GPIO_write_high(&PORTD, SEG_LATCH);
    // Wait 1 us
	_delay_us(1);
}

/**********************************************************************
 * Function: SEG_clear()
 **********************************************************************/
void SEG_clear(void)
{
	GPIO_write_low(&PORTD, SEG_LATCH);
	GPIO_write_low(&PORTD, SEG_CLK);
	GPIO_write_high(&PORTB, SEG_DATA);
	for (uint8_t i = 0; i != 8; i++)
		SEG_clk_2us();		
	GPIO_write_low(&PORTB, SEG_DATA);
	for (uint8_t i = 0; i != 8; i++)
		SEG_clk_2us();
	_delay_us(1);
}


/**********************************************************************
 * Function: SEG_clk_2us()
 **********************************************************************/
void SEG_clk_2us(void)
{
	_delay_us(1);
	GPIO_write_high(&PORTD, SEG_CLK);
	_delay_us(1);
	GPIO_write_low(&PORTD, SEG_CLK);
}