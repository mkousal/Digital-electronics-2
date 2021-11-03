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

static volatile uint8_t btn = 0;	// Initialize pressed button register


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

	uart_puts_P("--- Interactive UART console ---"); uart_puts_P("\r\n");
	uart_puts_P("1: ");
	uart_puts_P("\033[94m"); uart_puts_P("read "); 
	uart_puts_P("\033[0m"); uart_puts_P("current Timer/counter1 value"); uart_puts_P("\r\n");
	uart_puts_P("2: ");
	uart_puts_P("\033[31m"); uart_puts_P("reset ");
	uart_puts_P("\033[0m"); uart_puts_P("Timer/counter1"); uart_puts_P("\r\n");
	uart_puts_P("3: ");
	uart_puts_P("\033[92m"); uart_puts_P("read ");
	uart_puts_P("\033[0m"); uart_puts_P("ADC value from buttons"); uart_puts_P("\r\n");
	uart_puts_P("\033[91m"); uart_puts_P("> "); uart_puts_P("\033[0m");

    // Infinite loop
    while (1)
    {
		uint8_t c = uart_getc();
		if (c != '\0')
		{
			uart_putc(c); uart_puts_P("\r\n");
			switch (c) {
				case '1': ;
					char str[5];
					cli();
					int16_t temp = TCNT1;
					sei();
					itoa(temp, str, 10);
					uart_puts(str); uart_puts_P("\r\n");
					break;
				case '2':
					
					break;
				case '3':
				
					break;
				default:
					break;
			}
			uart_puts_P("\r\n"); uart_puts_P("\033[91m"); uart_puts_P("> "); uart_puts_P("\033[0m");

		}


		
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
	lcd_gotoxy(8, 0); lcd_puts("    ");		// LCD draw decimal value
	lcd_gotoxy(8, 0); lcd_puts(str);
	itoa(adc, str, 16);
	lcd_gotoxy(13, 0); lcd_puts("   ");		// LCD draw hexadecimal value
	lcd_gotoxy(13, 0); lcd_puts(str);
	lcd_gotoxy(8, 1); lcd_puts("      ");	// LCD clean button name area
	lcd_gotoxy(8, 1);
	if (adc < 50)	// Right (btn = 1)
	{
		btn = 1;
		lcd_puts_P("Right");
	}
	else if (adc > 51 && adc < 170)	// Up (btn = 2)
	{
		btn = 2;
		lcd_puts_P("Up");
	}
	else if (adc > 171 && adc < 320) // Down (btn = 3)
	{
		btn = 3;
		lcd_puts_P("Down");
	}
	else if (adc > 321 && adc < 520) // Left (btn = 4)
	{
		btn = 4;
		lcd_puts_P("Left");
	}
	else if (adc > 521 && adc < 800) // Select (btn = 5)
	{
		btn = 5;
		lcd_puts_P("Select");
	}
	else // none (btn = 0)
	{
		btn = 0;
		lcd_puts_P("none");
	}
}