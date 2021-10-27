/***********************************************************************
 * 
 * Stopwatch with LCD display output.
 * ATmega328P (Arduino Uno), 16 MHz, AVR 8-bit Toolchain 3.6.2
 *
 * Copyright (c) 2017-Present Tomas Fryza
 * Dept. of Radio Electronics, Brno University of Technology, Czechia
 * This work is licensed under the terms of the MIT license.
 * 
 **********************************************************************/

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

/* Includes ----------------------------------------------------------*/
#include <avr/io.h>         // AVR device-specific IO definitions
#include <avr/interrupt.h>  // Interrupts standard C library for AVR-GCC
#include "timer.h"          // Timer library for AVR-GCC
#include "lcd.h"            // Peter Fleury's LCD library
#include <stdlib.h>         // C library. Needed for conversion function
#include "util/delay.h"		// AVR delay library
#include "customChars.h"	// Custom LCD characters defines

void lcdUploadCustomChars()
{
	lcd_command(1 << LCD_CGRAM);
	for (uint8_t j = 0; j != sizeof(customChar); j++)
		lcd_data(customChar[j]);

}

/* Function definitions ----------------------------------------------*/
/**********************************************************************
 * Function: Main function where the program execution begins
 * Purpose:  Update stopwatch value on LCD display when 8-bit 
 *           Timer/Counter2 overflows.
 * Returns:  none
 **********************************************************************/
int main(void)
{
    // Initialize LCD display
    lcd_init(LCD_DISP_ON);
	
	lcdUploadCustomChars();

    // Put string(s) at LCD display
    lcd_gotoxy(0, 0);
    lcd_puts("Hello");
    lcd_putc('!');
	_delay_ms(500);
	lcd_clrscr();
	_delay_us(750);
	lcd_gotoxy(1, 0);
	lcd_puts("00:00.0");
	lcd_gotoxy(11, 0);
	lcd_putc('a');
	lcd_gotoxy(1, 1);
	lcd_putc('b');
	lcd_putc(0);
	lcd_gotoxy(11, 1);
	lcd_putc('c');
	_delay_ms(2000);

    // Configure 8-bit Timer/Counter2 for Stopwatch
    // Set the overflow prescaler to 16 ms and enable interrupt
	TIM2_16m();
	TIM2_overflow_interrupt_enable();
	
	TIM0_overflow_16ms();
	TIM0_overflow_interrupt_enable();
	
	TIM1_overflow_262ms();
	TIM1_overflow_interrupt_enable();

    // Enables interrupts by setting the global interrupt mask
    sei();

    // Infinite loop
    while (1)
    {
        /* Empty loop. All subsequent operations are performed exclusively 
         * inside interrupt service routines ISRs */
    }

    // Will never reach this
    return 0;
}

void lcdUpdateNumber(uint8_t num, uint8_t pos)
{
	char lcd_str[2] = "  ";
	itoa(num, lcd_str, 10);
	if (num < 10)
	{
		lcd_gotoxy(1 + pos*3, 0);
		if (pos == 2)
		{
			lcd_putc(lcd_str[0]);
		}
		else
		{
			lcd_putc('0');
			lcd_putc(lcd_str[0]);
		}
	} 
	else
	{
		lcd_gotoxy(1 + pos*3, 0);
		if (pos == 2)
			lcd_putc(lcd_str[0]);
		else
			lcd_puts(lcd_str);
	}
}

void lcdShowSquareValue(uint8_t a)
{
	char buff[4] = "    ";
	lcd_gotoxy(11, 0);
	lcd_puts("    ");
	lcd_gotoxy(11, 0);
	itoa(a*a, buff, 10);
	lcd_puts(buff);
}

/* Interrupt service routines ----------------------------------------*/
/**********************************************************************
 * Function: Timer/Counter2 overflow interrupt
 * Purpose:  Update the stopwatch on LCD display every sixth overflow,
 *           ie approximately every 100 ms (6 x 16 ms = 100 ms).
 **********************************************************************/
ISR(TIMER2_OVF_vect)
{
    static uint8_t number_of_overflows = 0;
	static uint8_t tens, secs, mins = 0;
	
    number_of_overflows++;
    if (number_of_overflows >= 6)
    {
        // Do this every 6 x 16 ms = 100 ms
        number_of_overflows = 0;
		tens++;
		lcdUpdateNumber(tens, 2);
		
		if (tens == 10)
		{
			tens = 0;
			secs++;
			lcdUpdateNumber(tens, 2);
			lcdUpdateNumber(secs, 1);
			lcdShowSquareValue(secs);
			if (secs == 60)
			{
				mins++;
				secs = 0;
				lcdUpdateNumber(secs, 1);
				lcdUpdateNumber(mins, 0);
			}
			
			if (mins == 60)
			{
				mins = 0;
				lcdUpdateNumber(mins, 0);
			}
		}
		
    }
    // Else do nothing and exit the ISR
}

ISR(TIMER0_OVF_vect) // Runs every 16 ms, 6*16 = 96ms (:=) 100ms
{
	static uint8_t pos, sym = 0;
	lcd_gotoxy(1 + pos, 1);
	lcd_putc(sym);
	sym++;
	if(sym == 6)
	{
		sym = 0;
		pos++;
		if (pos == 10)
		{
			pos = 0;
			lcd_gotoxy(1, 1);
			lcd_puts("          ");
		}
	}
}

ISR(TIMER1_OVF_vect) // Runs every 262 ms, coordinates for c = (11, 1)
{
	static uint8_t running_text[] = "   I like Digital Electronics\n";
	static uint8_t cnt, cnt1 = 0;
	cnt++;
	if (cnt == 2)
	{
		cnt = 0;
		cnt1++;
		lcd_gotoxy(11, 1);
		for (uint8_t i = cnt1; i != cnt1 + 5; i++)
		{
			lcd_putc(running_text[i]);
			if (running_text[i] == 10){
				cnt1 = 0;
				break;
			}
		}

	}
}