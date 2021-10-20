/***********************************************************************
 * 
 * Decimal counter with 7-segment output.
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
#include "segment.h"        // Seven-segment display library for AVR-GCC

/* Function definitions ----------------------------------------------*/
/**********************************************************************
 * Function: Main function where the program execution begins
 * Purpose:  Display decimal counter values on SSD (Seven-segment 
 *           display) when 16-bit Timer/Counter1 overflows.
 * Returns:  none
 **********************************************************************/
int main(void)
{
    // Configure SSD signals
    SEG_init();
    // Test of SSD: display number '3' at position 0
    SEG_update_shift_regs(3, 0, 0);

    // Configure 16-bit Timer/Counter1 for Decimal counter
    // Set the overflow prescaler to 262 ms and enable interrupt
	TIM1_overflow_262ms();
	TIM1_overflow_interrupt_enable();

	TIM0_overflow_1ms();
	TIM0_overflow_interrupt_enable();

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

volatile uint8_t cnt[] = {0,0,0,0};

/* Interrupt service routines ----------------------------------------*/
/**********************************************************************
 * Function: Timer/Counter1 overflow interrupt
 * Purpose:  Increment decimal counter value and display it on SSD.
 **********************************************************************/
ISR(TIMER1_OVF_vect)
{
	cnt[0]++;	
		
	if (cnt[0] == 10)
	{
		cnt[0] = 0;
		cnt[1]++;	
	}
	if (cnt[1] == 6)
	{
		cnt[1] = 0;
		cnt[2]++;
	}
	if (cnt[2] == 10)
	{
		cnt[2] = 0;
		cnt[3]++;
	}
	if (cnt[3] == 6)
	{
		cnt[3] = 0;
	}
}

ISR(TIMER0_OVF_vect)
{
	static uint8_t pos = 0;
	static int8_t dir = 1;
	if (pos != 2)
		SEG_update_shift_regs(cnt[pos], pos, 0);
	else
		SEG_update_shift_regs(cnt[pos], pos, 1);
	pos += dir;
	if (pos == 3)
		dir = -1;
	if (pos == 0)
		dir = 1;
}