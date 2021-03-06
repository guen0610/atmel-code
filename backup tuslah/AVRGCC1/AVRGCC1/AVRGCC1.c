#ifndef F_CPU
#define F_CPU 8000000UL // 1 MHz clock speed
#endif


#include <avr/io.h>
#include <util/delay.h>
#include "lcd.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#define RELAY_PIN 1
volatile char SHOWA [3];
volatile int16_t COUNTA = 0;
volatile int sample=0;
volatile unsigned char value; 
// Define baud rate
#define USART0_BAUDRATE 9600   
#define BAUD_PRESCALE (((F_CPU / (USART0_BAUDRATE * 16UL))) - 1)

// High when a value is ready to be read
volatile int readFlag;

// Value to store analog result
volatile uint16_t analogVal;

ISR(ADC_vect);

int main(void)
{
	DDRA |= (1<<RELAY_PIN); //relay pin
	PORTA &= ~(1<<RELAY_PIN);
    DDRC = 0xFF;    // lcd pins
  
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
	
	ADMUX |= (1<<MUX1)|(1<<MUX0)|(1<<REFS0);
	ADCSRA |= (1<<ADEN)|(1<<ADIE)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
	//USART0_Init();
	_delay_ms(1000);
	lcd_init_4d();
	lcd_write_instruction_4d(lcd_Clear);
    _delay_ms(10);
	lcd_write_string_4d("sdsd");
	
    int x=0;
   /* while(1)
    {
    	lcd_write_instruction_4d(lcd_Clear);
		_delay_ms(10);
    	x++;
    	itoa(x,SHOWA,10);
		lcd_write_string_4d("AA");
		_delay_ms(1000);
	}*/
	sei();
	MCUSR |= (1<<SE)|(1<<SM0); //adc noise reduction mode
	ADCSRA |= (1<<ADSC);
	
	while(1)
	{
		// Check to see if the value has been updated
		if (readFlag == 1){
    
		// Perform whatever updating needed
			
		readFlag = 0;
		}
  
  // Whatever else you would normally have running in loop().
  		
	}
	
	
	
}
ISR(ADC_vect)
{
		memset(SHOWA,'\0',3);
			lcd_write_instruction_4d(lcd_Clear);
			_delay_ms(10);
    		COUNTA = (ADC*500)/1024;
    		itoa(COUNTA,SHOWA,10);
			lcd_write_string_4d("ROOM1: ");
			lcd_write_string_4d(SHOWA);
			lcd_write_string_4d("C");
			_delay_ms(500);
			ADCSRA |= (1<<ADSC);
	/* // Done reading
  readFlag = 1;
  
  // Must read low first
  analogVal = ADCH;
  //itoa(ADCH,SHOWA,10);
			//lcd_write_string_4d(SHOWA);
  ADCSRA |= (1<<ADSC);*/
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
