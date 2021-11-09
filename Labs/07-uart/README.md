# Lab 7: Martin Kousal

Link to this file in your GitHub repository:

[https://github.com/mkousal/Digital-electronics-2/blob/main/Labs/07-uart/README.md](https://github.com/mkousal/Digital-electronics-2/blob/main/Labs/07-uart/README.md)


### Analog-to-Digital Conversion

1. Complete table with voltage divider, calculated, and measured ADC values for all five push buttons.

   | **Push button** | **PC0[A0] voltage** | **ADC value (calculated)** | **ADC value (measured)** |
   | :-: | :-: | :-: | :-: |
   | Right  | 0&nbsp;V     | 0    | 0    |
   | Up     | 0.495&nbsp;V | 101  | 99   |
   | Down   | 1.203&nbsp;V | 246  | 256  |
   | Left   | 1.97&nbsp;V  | 403  | 408  |
   | Select | 3.182&nbsp;V | 651  | 640  |
   | none   | 5&nbsp;V     | 1023 | 1023 |

2. Code listing of ACD interrupt service routine for sending data to the LCD/UART and identification of the pressed button. Always use syntax highlighting and meaningful comments:

```c
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
```


### UART communication

1. (Hand-drawn) picture of UART signal when transmitting three character data `De2` in 4800 7O2 mode (7 data bits, odd parity, 2 stop bits, 4800&nbsp;Bd).

   ![your figure](images/wavedrom.svg)

2. Flowchart figure for function `uint8_t get_parity(uint8_t data, uint8_t type)` which calculates a parity bit of input 8-bit `data` according to parameter `type`. The image can be drawn on a computer or by hand. Use clear descriptions of the individual steps of the algorithms.

   ![your figure](images/flowchart.svg)

### Temperature meter

Consider an application for temperature measurement and display. Use temperature sensor [TC1046](http://ww1.microchip.com/downloads/en/DeviceDoc/21496C.pdf), LCD, one LED and a push button. After pressing the button, the temperature is measured, its value is displayed on the LCD and data is sent to the UART. When the temperature is too high, the LED will start blinking.

1. Scheme of temperature meter. The image can be drawn on a computer or by hand. Always name all components and their values.

   ![your figure](images/schematic.svg)