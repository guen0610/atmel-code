#define F_CPU 1000000UL // 1 MHz clock speed


#include <avr/io.h>
#include <util/delay.h>
#include "lcd.h"

#define USART0_BAUDRATE 9600   
#define BAUD_PRESCALE (((F_CPU / (USART0_BAUDRATE * 16UL))) - 1)
volatile unsigned char coming; 

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
    USART0_Init();
	_delay_ms(1000);
    lcd_init_4d();
	lcd_write_instruction_4d(lcd_Clear);
    _delay_ms(10);
	lcd_write_string_4d("Mainboard");
	sei();
	while(1)
	{
		_delay_ms(2000);
		lcd_write_instruction_4d(lcd_Clear);
		_delay_ms(10);
		lcd_write_string_4d("ROOM1: ");
		lcd_write_string_4d(coming);
		lcd_write_string_4d("C");
	}
//***************************************************************************

}
void USART0_Init(void){
   // Set baud rate
   UBRR0L = BAUD_PRESCALE;// Load lower 8-bits into the low byte of the UBRR register
   UBRR0H = (BAUD_PRESCALE >> 8); 
	 /* Load upper 8-bits into the high byte of the UBRR register
    Default frame format is 8 data bits, no parity, 1 stop bit
  to change use UCSRC, see AVR datasheet*/ 

  // Enable receiver and transmitter and receive complete interrupt 
  UCSR0B = ((1<<TXEN)|(1<<RXEN)|(1<<RXCIE));
}

uint8_t USART0_ReceiveByte(){
  while((UCSR0A &(1<<RXC)) == 0);
  return UDR0;
}
ISR(USART0_RXC_vect){
 
   coming = UDR0;             //read UART register into value
}