/***********************************************************************
 * 
 * The I2C bus scanner detects the addresses of the modules that are 
 * connected to the SDA and SCL signals. A simple description of FSM is 
 * used.
 * ATmega328P (Arduino Uno), 16 MHz, AVR 8-bit Toolchain 3.6.2
 *
 * Copyright (c) 2017-Present Tomas Fryza
 * Dept. of Radio Electronics, Brno University of Technology, Czechia
 * This work is licensed under the terms of the MIT license.
 * 
 **********************************************************************/

/* Defines -----------------------------------------------------------*/
#ifndef F_CPU
# define F_CPU 16000000  // CPU frequency in Hz required for UART_BAUD_SELECT
#endif

/* Includes ----------------------------------------------------------*/
#include <avr/io.h>         // AVR device-specific IO definitions
#include <avr/interrupt.h>  // Interrupts standard C library for AVR-GCC
#include "timer.h"          // Timer library for AVR-GCC
#include <stdlib.h>         // C library. Needed for conversion function
#include "uart.h"           // Peter Fleury's UART library
#include "twi.h"            // TWI library for AVR-GCC
#include <util/delay.h>		// AVR delay library

/* Variables ---------------------------------------------------------*/
typedef enum {              // FSM declaration
    STATE_IDLE = 1,
    STATE_SEND,
    STATE_ACK
} state_t;

/* Function definitions ----------------------------------------------*/
/**********************************************************************
 * Function: Main function where the program execution begins
 * Purpose:  Use Timer/Counter1 and send I2C (TWI) address every 33 ms.
 *           Send information about scanning process to UART.
 * Returns:  none
 **********************************************************************/
int main(void)
{
    // Initialize I2C (TWI)
    twi_init();

    // Initialize UART to asynchronous, 8N1, 9600
    uart_init(UART_BAUD_SELECT(9600, F_CPU));

    // Configure 16-bit Timer/Counter1 to update FSM
    // Set prescaler to 33 ms and enable interrupt
    TIM1_overflow_33ms();
    TIM1_overflow_interrupt_enable();

    // Enables interrupts by setting the global interrupt mask
    sei();

    // Put strings to ringbuffer for transmitting via UART
    uart_puts("\r\nScan I2C-bus for devices:\r\n");
	uart_puts_P("\r\n      .0 .1 .2 .3 .4 .5 .6 .7 .8 .9 .a .b .c .d .e .f\r\n");
	uart_puts_P("0x0.: RA RA RA RA RA RA RA RA ");
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
 * Purpose:  Update Finite State Machine and test I2C slave addresses 
 *           between 8 and 119.
 **********************************************************************/
ISR(TIMER1_OVF_vect)
{
    static state_t state = STATE_IDLE;  // Current state of the FSM
    static uint8_t addr = 7;            // I2C slave address
    uint8_t result = 1;                 // ACK result from the bus
    char uart_string[2] = "00"; // String for converting numbers by itoa()
	static uint8_t cnt = 0;	// Counter of detected devices
	
    // FSM
    switch (state)
    {
    // Increment I2C slave address
    case STATE_IDLE:
        addr++;
		if (addr == 120)
		{
			char cnt_str[3] = "000";
			uart_puts_P("RA RA RA RA RA RA RA RA\r\n\r\n");
			uart_puts_P("Number of detected devices: ");
			itoa(cnt, cnt_str, 10);
			uart_puts(cnt_str);
			uart_puts_P("\r\n");
			cnt = 0;
			addr = 7;
			uart_puts("\r\nScan I2C-bus for devices:\r\n");
			uart_puts_P("\r\n      .0 .1 .2 .3 .4 .5 .6 .7 .8 .9 .a .b .c .d .e .f\r\n");
			uart_puts_P("0x0.: RA RA RA RA RA RA RA RA ");
			_delay_ms(500);
		}
		if (addr > 7 && addr < 120)
			state = STATE_SEND;
        // If slave address is between 8 and 119 then move to SEND state

        break;
    
    // Transmit I2C slave address and get result
    case STATE_SEND:
        // I2C address frame:
        // +------------------------+------------+
        // |      from Master       | from Slave |
        // +------------------------+------------+
        // | 7  6  5  4  3  2  1  0 |     ACK    |
        // |a6 a5 a4 a3 a2 a1 a0 R/W|   result   |
        // +------------------------+------------+
        result = twi_start((addr<<1) + TWI_WRITE);
        twi_stop();
        /* Test result from I2C bus. If it is 0 then move to ACK state, 
         * otherwise move to IDLE */
		if (result == 0)
			state = STATE_ACK;
		else
		{
			uint8_t tmp = addr % 16;
			uint8_t tmp_addr = addr / 15;
			char tmp_str[1] = " ";
			itoa(tmp_addr, tmp_str, 10);
			if (tmp != 0)
			{
				uart_puts_P("-- ");
			}
			else
			{
				uart_puts_P("--\r\n0x");
				uart_puts(tmp_str);
				uart_puts_P(".: ");
				
			}
			state = STATE_IDLE;
		}
			
        break;

    // A module connected to the bus was found
    case STATE_ACK:
        // Send info about active I2C slave to UART and move to IDLE
		cnt++;
		itoa(addr, uart_string, 16);
		uart_puts(uart_string);
		uart_puts_P(" ");
		state = STATE_IDLE;		
        break;

    // If something unexpected happens then move to IDLE
    default:
        state = STATE_IDLE;
        break;
    }
}