#define F_CPU 1000000UL // 1 MHz clock speed


#include <avr/io.h>
#include <util/delay.h>
#include "lcd.h"
#include "uart0.h"
#include <avr/interrupt.h>

char * coming;

#include <avr/io.h>

int main(void)
{
	DDRA |= (1<<1); //relay pin
	PORTA &= ~(_BV(1));
    DDRC = 0xFF;    // lcd pins
	PORTC &= ~(_BV(3));
    PORTC &= ~(_BV(2));
    PORTC &= ~(_BV(1));
    PORTC &= ~(_BV(0));
 
// configure the microprocessor pins for the control lines
    lcd_E_ddr |= (1<<lcd_E_bit);                    // E line - output
    lcd_RS_ddr |= (1<<lcd_RS_bit);                  // RS line - output
 
    lcd_RS_port &= ~(1<<lcd_RS_bit);
	_delay_ms(1000);
    lcd_init_4d();
	lcd_write_instruction_4d(lcd_Clear);
    _delay_ms(10);
	lcd_write_string_4d("Mainboard");
	
	sei();

	while(1)
	{
		_delay_ms(2000);
		if(UDataAvailable0() != 0)
		{
			coming = UReadData0();
		}
		lcd_write_instruction_4d(lcd_Clear);
		_delay_ms(10);
		lcd_write_string_4d("ROOM1: ");
		lcd_write_string_4d(coming);
		lcd_write_string_4d("C");

	}
//***************************************************************************

}
