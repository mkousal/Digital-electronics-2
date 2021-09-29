/*
 * 02-leds_knightRider.c
 *
 * Created: 29.09.2021 13:15:43
 * Author : Martin
 */ 
#ifndef F_CPU
#define F_CPU 16000000
#endif

#define LED_1 PB0
#define LED_2 PB1
#define LED_3 PB2
#define LED_4 PB3
#define LED_5 PB4
#define DELAY 150

#include <avr/io.h>
#include <util/delay.h>
#include <avr/sfr_defs.h>

int main(void)
{
    DDRB |= (1<<LED_1) | (1<<LED_2) | (1<<LED_3) | (1<<LED_4) | (1<<LED_5);
	PORTB &= ~((1<<LED_1) | (1<<LED_2) | (1<<LED_3) | (1<<LED_4) | (1<<LED_5));
	uint8_t led[] = {LED_1, LED_2, LED_3,LED_4, LED_5};
    while (1) 
    {
		for(uint8_t i = 0; i != 5; i++){
			PORTB |= (1<<led[i]);
			if (i != 0){
				PORTB &= ~(1<<led[i-1]);
 				_delay_ms(DELAY);
			}
		}
		for(uint8_t i = 4; i != 255; --i){
			PORTB |= (1<<led[i]);
			if (i != 4){
				PORTB &= ~(1<<led[i+1]);
				_delay_ms(DELAY);
			}
		}
    }
}

