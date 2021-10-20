/*
 * gpio.c
 *
 * Created: 06.10.2021 11:09:34
 *  Author: Martin Kousal
 */ 
#include "gpio.h"

void GPIO_config_output (volatile uint8_t *reg_name, uint8_t pin_num)
{
	*reg_name |= (1<<pin_num);
}

void GPIO_config_input_nopull (volatile uint8_t *reg_name, uint8_t pin_num)
{
	*reg_name &= ~(1<<pin_num);
	reg_name++;
	*reg_name &= ~(1<<pin_num);
}

void GPIO_config_input_pullup (volatile uint8_t *reg_name, uint8_t pin_num)
{
	*reg_name &= ~(1<<pin_num);
	reg_name++;
	*reg_name |= (1<<pin_num);
}

void GPIO_write_low (volatile uint8_t *reg_name, uint8_t pin_num)
{
	*reg_name &= ~(1<<pin_num);
}

void GPIO_write_high (volatile uint8_t *reg_name, uint8_t pin_num)
{
	*reg_name |= (1<<pin_num);
}

void GPIO_toggle (volatile uint8_t *reg_name, uint8_t pin_num)
{
	*reg_name ^= (1<<pin_num);
}

uint8_t GPIO_read (volatile uint8_t *reg_name, uint8_t pin_num)
{
	return ((*reg_name & (1<<pin_num)) != 0) ? 1 : 0; 
}