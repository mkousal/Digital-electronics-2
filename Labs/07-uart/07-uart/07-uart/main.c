/***********************************************************************
 * 
 * Analog-to-digital conversion with displaying result on LCD and 
 * transmitting via UART.
 * ATmega328P (Arduino Uno), 16 MHz, AVR 8-bit Toolchain 3.6.2
 *
 * Copyright (c) 2018-Present Tomas Fryza
 * Dept. of Radio Electronics, Brno University of Technology, Czechia
 * This work is licensed under the terms of the MIT license.
 * 
 **********************************************************************/

/* Includes ----------------------------------------------------------*/
#include <avr/io.h>         // AVR device-specific IO definitions
#include <avr/interrupt.h>  // Interrupts standard C library for AVR-GCC
#include "timer.h"          // Timer library for AVR-GCC
#include "lcd.h"            // Peter Fleury's LCD library
#include <stdlib.h>         // C library. Needed for conversion function
#include "uart.h"           // Peter Fleury's UART library

/* Function definitions ----------------------------------------------*/
/**********************************************************************
 * Function: Main function where the program execution begins
 * Purpose:  Use Timer/Counter1 and start ADC conversion four times 
 *           per second. Send value to LCD and UART.
 * Returns:  none
 **********************************************************************/
int main(void)
{
    // Initialize LCD display
    lcd_init(LCD_DISP_ON);
    lcd_gotoxy(1, 0); lcd_puts("value:");
    lcd_gotoxy(3, 1); lcd_puts("key:");
    lcd_gotoxy(8, 0); lcd_puts("a");    // Put ADC value in decimal
    lcd_gotoxy(13,0); lcd_puts("b");    // Put ADC value in hexadecimal
    lcd_gotoxy(8, 1); lcd_puts("c");    // Put button name here

    // Configure ADC to convert PC0[A0] analog value
    // Set ADC reference to AVcc
	ADMUX &= ~(1<<REFS1); 
	ADMUX |= (1<<REFS0);
    // Set input channel to ADC0
	ADMUX &= ~((1<<MUX3) | (1<<MUX2) | (1<<MUX1) | (1<<MUX0));
    // Enable ADC module
	ADCSRA |= (1<<ADEN);
    // Enable conversion complete interrupt
	ADCSRA |= (1<<ADIE);
    // Set clock prescaler to 128
	ADCSRA |= (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
    // Configure 16-bit Timer/Counter1 to start ADC conversion
    // Set prescaler to 262 ms and enable overflow interrupt
	TIM1_overflow_262ms();
	TIM1_overflow_interrupt_enable();
    // Initialize UART to asynchronous, 8N1, 9600
	uart_init(UART_BAUD_SELECT(9600, 16000000UL));
    // Enables interrupts by setting the global interrupt mask
    sei();

	uart_puts("\033[4;32m");        // 4: underline style; 32: green foreground
	uart_puts("This is all Green and Underlined.");
	uart_puts("\033[0m");           // 0: reset all attributes
	uart_puts("This is Normal text again.");

    // Infinite loop
    while (1)
    {
        /* Empty loop. All subsequent operations are performed exclusively 
         * inside interrupt service routines ISRs */
    }

    // Will never reach this
    return 0;
}

/* Interrupt service routines ----------------------------------------*/
/**********************************************************************
 * Function: Timer/Counter1 overflow interrupt
 * Purpose:  Use single conversion mode and start conversion four times
 *           per second.
 **********************************************************************/
ISR(TIMER1_OVF_vect)
{
    // Start ADC conversion
	ADCSRA |= (1<<ADSC);
}

/**********************************************************************
 * Function: ADC complete interrupt
 * Purpose:  Display value on LCD and send it to UART.
 **********************************************************************/
ISR(ADC_vect)
{
	uint16_t adc = ADC;		// Read ADC value from register
	char str[4] = "    ";	// Initialize char array for int2char conversion
	itoa(adc, str, 10);		// Convert int to decimal char array
	uart_puts(str);			// Send it to UART
	uart_putc(' ');
	lcd_gotoxy(8, 0); lcd_puts("    ");		// LCD draw decimal value
	lcd_gotoxy(8, 0); lcd_puts(str);
	itoa(adc, str, 16);
	lcd_gotoxy(13, 0); lcd_puts("   ");		// LCD draw hexadecimal value
	lcd_gotoxy(13, 0); lcd_puts(str);
	lcd_gotoxy(8, 1); lcd_puts("      ");	// LCD clean button name area
	lcd_gotoxy(8, 1);
	uart_puts(str);	// UART print hexadecimal value
	uart_putc(' ');
	if (adc < 50)	// Right
	{
		lcd_puts("Right");
		uart_puts("Right");
	}
	else if (adc > 51 && adc < 170)	// Up
	{
		lcd_puts("Up");
		uart_puts("Up");
	}
	else if (adc > 171 && adc < 320) // Down
	{
		lcd_puts("Down");
		uart_puts("Down");
	}
	else if (adc > 321 && adc < 520) // Left
	{
		lcd_puts("Left");
		uart_puts("Left");
	}
	else if (adc > 521 && adc < 800) // Select
	{
		lcd_puts("Select");
		uart_puts("Select");
	}
	else // none
	{
		lcd_puts("none");
		uart_puts("none");
	}
	
	uart_puts("\r\n");		// Send new line
}