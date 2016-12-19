#define F_CPU 1000000UL // 1 MHz clock speed


#include <avr/io.h>
#include <util/delay.h>
#include "lcd.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <avr/sleep.h>

volatile char SHOWA [3];
volatile int16_t COUNTA = 0;
volatile int sample=0;
volatile unsigned char value; 
// Define baud rate
#define USART0_BAUDRATE 9600   
#define BAUD_PRESCALE (((F_CPU / (USART0_BAUDRATE * 16UL))) - 1)



ISR(ADC_vect);

int main(void)
{
	DDRA |= (1<<1); //relay pin
    DDRC = 0xFF;    // lcd pins
    _delay_ms(1000);
   
	DDRF &= ~(_BV(3));
	_delay_ms(80);
	PORTC &= ~(_BV(3));
    PORTC &= ~(_BV(2));
    PORTC &= ~(_BV(1));
    PORTC &= ~(_BV(0));
 
// configure the microprocessor pins for the control lines
    lcd_E_ddr |= (1<<lcd_E_bit);                    // E line - output
    lcd_RS_ddr |= (1<<lcd_RS_bit);                  // RS line - output
 
    lcd_RS_port &= ~(1<<lcd_RS_bit);
	PORTF &= ~(_BV(1));
	
	ADMUX |= (1<<ADLAR)|(1<<MUX1)|(1<<MUX0);
	ADCSRA = 0X8F;
	USART0_Init();
	_delay_ms(1000);
	lcd_init_4d();
	lcd_write_instruction_4d(lcd_Clear);
    _delay_ms(10);
	lcd_write_string_4d("Starting...");
	sei();
	MCUSR |= (1<<SE)|(1<<SM0); //adc noise reduction mode
	ADCSRA |= (1<<ADSC);
	
	   
	
	while(1)
	{
		sleep_cpu();
		/*COUNTA = adc_read(ADC_PRESCALER_128, ADC_VREF_AVCC, 3);
		itoa(COUNTA,SHOWA,10);
		lcd_write_instruction_4d(lcd_Clear);
		_delay_ms(10);
		lcd_write_string_4d(SHOWA);
		_delay_ms(2000);*/
	}
}
ISR(ADC_vect)
{
	lcd_write_instruction_4d(lcd_Clear);
	_delay_ms(10);
	/*if((ADCH-COUNTA)>5)
	{
		int16_t x = rand(4);
		COUNTA += x;
	}		
	else if((COUNTA-ADCH)>5)
	{
		int16_t y = rand(4);
		COUNTA -= y;
	}		
	else 		*/
		COUNTA = ADCH;
	
	itoa(COUNTA,SHOWA,10);
	lcd_write_string_4d("ROOM1: ");
	lcd_write_string_4d(SHOWA);
	lcd_write_string_4d("C");
	USART0_SendByte(SHOWA);  // send value 
	_delay_ms(2000);
	ADCSRA |= (1<<ADSC);
}

void USART0_Init(void){
   // Set baud rate
   UBRR0L = BAUD_PRESCALE;// Load lower 8-bits into the low byte of the UBRR register
   UBRR0H = (BAUD_PRESCALE >> 8); 
	 /* Load upper 8-bits into the high byte of the UBRR register
    Default frame format is 8 data bits, no parity, 1 stop bit
  to change use UCSRC, see AVR datasheet*/ 

  // Enable receiver and transmitter and receive complete interrupt 
  UCSR0B = (1<<TXEN);
}


void USART0_SendByte(uint8_t u8Data){

  // Wait until last byte has been transmitted
  while((UCSR0A &(1<<UDRE0)) == 0);

  // Transmit data
  UDR0 = u8Data;
}
