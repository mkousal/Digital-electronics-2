/*
 * gpio.h
 *
 * Created: 06.10.2021 11:09:22
 *  Author: Martin Kousal
 */ 


#ifndef GPIO_H_
#define GPIO_H_

#include <avr/io.h>


/* Function prototypes -----------------------------------------------*/
/**
 * @name Functions
 */

/* GPIO_config_output */

/**
 * @brief  Configure one output pin in Data Direction Register.
 * @param  reg_name Address of Data Direction Register, such as &DDRB
 * @param  pin_num  Pin designation in the interval 0 to 7
 * @return none
 */
void GPIO_config_output (volatile uint8_t *reg_name, uint8_t pin_num);


/**
 * @brief  Configure one input pin and disable pull-up.
 * @param  reg_name Address of Data Direction Register, such as &DDRB
 * @param  pin_num  Pin designation in the interval 0 to 7
 * @return none
 */
void GPIO_config_input_nopull (volatile uint8_t *reg_name, uint8_t pin_num);


/**
 * @brief  Configure one input pin and disable pull-up.
 * @param  reg_name Address of Data Direction Register, such as &DDRB
 * @param  pin_num  Pin designation in the interval 0 to 7
 * @return none
 */
void GPIO_config_input_pullup (volatile uint8_t *reg_name, uint8_t pin_num);


/**
 * @brief  Write one pin to a low value.
 * @param  reg_name Address of Port Register, such as &PORTB
 * @param  pin_num  Pin designation in the interval 0 to 7
 * @return none
 */
void GPIO_write_low (volatile uint8_t *reg_name, uint8_t pin_num);


/**
 * @brief  Write one pin to a high value.
 * @param  reg_name Address of Port Register, such as &PORTB
 * @param  pin_num  Pin designation in the interval 0 to 7
 * @return none
 */
void GPIO_write_high (volatile uint8_t *reg_name, uint8_t pin_num);

/**
 * @brief  Toggle one selected pin.
 * @param  reg_name Address of Port Register, such as &PORTB
 * @param  pin_num  Pin designation in the interval 0 to 7
 * @return none
 */
void GPIO_toggle (volatile uint8_t *reg_name, uint8_t pin_num);


/**
 * @brief  Read a value from input pin.
 * @param  reg_name Address of Pin Register, such as &PINB
 * @param  pin_num  Pin designation in the interval 0 to 7
 * @return Pin value
 */
uint8_t GPIO_read (volatile uint8_t *reg_name, uint8_t pin_num);

#endif /* GPIO_H_ */