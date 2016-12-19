
#define F_CPU 1000000UL // 1 MHz clock speed


#include <avr/pgmspace.h>
#include <inttypes.h>
#include <avr/io.h>
#include <util/delay.h>

#define lcd_D7_port     PORTC                   // lcd D7 connection
#define lcd_D7_bit      PORTC7
#define lcd_D7_ddr      DDRC

#define lcd_D6_port     PORTC                   // lcd D6 connection
#define lcd_D6_bit      PORTC6
#define lcd_D6_ddr      DDRC

#define lcd_D5_port     PORTC                   // lcd D5 connection
#define lcd_D5_bit      PORTC5
#define lcd_D5_ddr      DDRC

#define lcd_D4_port     PORTC                  // lcd D4 connection
#define lcd_D4_bit      PORTC4
#define lcd_D4_ddr      DDRC

#define lcd_E_port      PORTD                  // lcd Enable pin
#define lcd_E_bit       PORTD7
#define lcd_E_ddr       DDRD

/*#define lcd_RW_port      PORTD                   // lcd RW pin
#define lcd_RW_bit       PORTD6
#define lcd_RW_ddr       DDRD
*/

#define lcd_RS_port     PORTD                   // lcd Register Select pin
#define lcd_RS_bit      PORTD5
#define lcd_RS_ddr      DDRD

// LCD module information
#define lcd_LineOne     0x00                    // start of line 1
#define lcd_LineTwo     0x40                    // start of line 2
//#define   lcd_LineThree   0x14                  // start of line 3 (20x4)
//#define   lcd_lineFour    0x54                  // start of line 4 (20x4)
//#define   lcd_LineThree   0x10                  // start of line 3 (16x4)
//#define   lcd_lineFour    0x50                  // start of line 4 (16x4)

// LCD instructions
#define lcd_Clear           0b00000001          // replace all characters with ASCII 'space'
#define lcd_Home            0b00000010          // return cursor to first position on first line
#define lcd_EntryMode       0b00000110          // shift cursor from left to right on read/write
#define lcd_DisplayOff      0b00001000          // turn display off
#define lcd_DisplayOn       0b00001100          // display on, cursor off, don't blink character
#define lcd_FunctionReset   0b00110000          // reset the LCD
#define lcd_FunctionSet4bit 0b00101000          // 4-bit data, 2-line display, 5 x 7 font
#define lcd_SetCursor       0b10000000          // set cursor position

void lcd_write_4(uint8_t);
void lcd_write_instruction_4d(uint8_t);
void lcd_write_character_4d(uint8_t);
void lcd_write_string_4d(uint8_t *);
void lcd_init_4d(void);
void lcd_puthex(const char data);
void CopyStringtoLCD(const uint8_t *FlashLoc);
void LCDsendChar(uint8_t ch);


void lcd_puthex(const char data)
{
	
	//wait for the LCD to be ready
	uint8_t temp= '0';
	_delay_us(80);
	//print '0'
	//lcd_write_character_4d('0');
	//_delay_us(80);
	//print 'x'
	//lcd_write_character_4d('x');
	//_delay_us(80);
	(((data>>4)&0x0F)<=9) ? (temp='0'+((data>>4)&0x0F)) : (temp='A'+ ((data>>4)&0x0F)-0x0A);
	_delay_us(80);
	//print high nibble as hex
	lcd_write_character_4d(temp);
	_delay_us(80);
	((data&0x0F)<=9) ? (temp='0'+ (data&0x0F)) : (temp='A'+ (data&0x0F)-0x0A);
	lcd_write_character_4d(temp);
	_delay_us(80);
	//print low nibble as hex
}



void lcd_init_4d(void)
{
// Power-up delay
    _delay_ms(100);                                 // initial 40 mSec delay

    lcd_RS_port &= ~(1<<lcd_RS_bit);                // select the Instruction Register (RS low)
    lcd_E_port &= ~(1<<lcd_E_bit);                  // make sure E is initially low

// Reset the LCD controller
    lcd_write_4(lcd_FunctionReset);                 // first part of reset sequence
    _delay_ms(10);                                  // 4.1 mS delay (min)

    lcd_write_4(lcd_FunctionReset);                 // second part of reset sequence
    _delay_ms(1);                                 // 100uS delay (min)

    lcd_write_4(lcd_FunctionReset);                 // third part of reset sequence
    _delay_ms(1);                                 // this delay is omitted in the data sheet

// Preliminary Function Set instruction - used only to set the 4-bit mode.
// The number of lines or the font cannot be set at this time since the controller is still in the
//  8-bit mode, but the data transfer mode can be changed since this parameter is determined by one 
//  of the upper four bits of the instruction.
 
    lcd_write_4(lcd_FunctionSet4bit);               // set 4-bit mode
    _delay_ms(1);                                  // 40uS delay (min)

// Function Set instruction
    lcd_write_instruction_4d(lcd_FunctionSet4bit);   // set mode, lines, and font
    _delay_ms(1);                                  // 40uS delay (min)

// The next three instructions are specified in the data sheet as part of the initialization routine, 
//  so it is a good idea (but probably not necessary) to do them just as specified and then redo them 
//  later if the application requires a different configuration.

// Display On/Off Control instruction
    lcd_write_instruction_4d(lcd_DisplayOff);        // turn display OFF
    _delay_ms(1);                                  // 40uS delay (min)

// Clear Display instruction
    lcd_write_instruction_4d(lcd_Clear);             // clear display RAM
    _delay_ms(4);                                   // 1.64 mS delay (min)

// ; Entry Mode Set instruction
    lcd_write_instruction_4d(lcd_EntryMode);         // set desired shift characteristics
    _delay_ms(1);                                  // 40uS delay (min)

// This is the end of the LCD controller initialization as specified in the data sheet, but the display
//  has been left in the OFF condition.  This is a good time to turn the display back ON.
 
// Display On/Off Control instruction
    lcd_write_instruction_4d(lcd_DisplayOn);         // turn the display ON
    _delay_ms(1);                                  // 40uS delay (min)
}

/*...........................................................................
  Name:     lcd_write_string_4d
; Purpose:  display a string of characters on the LCD
  Entry:    (theString) is the string to be displayed
  Exit:     no parameters
  Notes:    uses time delays rather than checking the busy flag
*/
void lcd_write_string_4d(uint8_t theString[])
{
    volatile int i = 0;                             // character counter*/
    while (theString[i] != 0)
    {
        lcd_write_character_4d(theString[i]);
        i++;
        _delay_us(80);                              // 40 uS delay (min)
    }
}

/*...........................................................................
  Name:     lcd_write_character_4d
  Purpose:  send a byte of information to the LCD data register
  Entry:    (theData) is the information to be sent to the data register
  Exit:     no parameters
  Notes:    does not deal with RW (busy flag is not implemented)
*/

void lcd_write_character_4d(uint8_t theData)
{
    lcd_RS_port |= (1<<lcd_RS_bit);                 // select the Data Register (RS high)
    lcd_E_port &= ~(1<<lcd_E_bit);                  // make sure E is initially low
    lcd_write_4(theData);                           // write the upper 4-bits of the data
    lcd_write_4(theData << 4);                      // write the lower 4-bits of the data
}

/*...........................................................................
  Name:     lcd_write_instruction_4d
  Purpose:  send a byte of information to the LCD instruction register
  Entry:    (theInstruction) is the information to be sent to the instruction register
  Exit:     no parameters
  Notes:    does not deal with RW (busy flag is not implemented)
*/
void lcd_write_instruction_4d(uint8_t theInstruction)
{
    lcd_RS_port &= ~(1<<lcd_RS_bit);                // select the Instruction Register (RS low)
    lcd_E_port &= ~(1<<lcd_E_bit);                  // make sure E is initially low
    lcd_write_4(theInstruction);                    // write the upper 4-bits of the data
    lcd_write_4(theInstruction << 4);               // write the lower 4-bits of the data
}


/*...........................................................................
  Name:     lcd_write_4
  Purpose:  send a byte of information to the LCD module
  Entry:    (theByte) is the information to be sent to the desired LCD register
            RS is configured for the desired LCD register
            E is low
            RW is low
  Exit:     no parameters
  Notes:    use either time delays or the busy flag
*/
void lcd_write_4(uint8_t theByte)
{
    lcd_D7_port &= ~(1<<lcd_D7_bit);                        // assume that data is '0'
    if (theByte & 1<<7) lcd_D7_port |= (1<<lcd_D7_bit);     // make data = '1' if necessary

    lcd_D6_port &= ~(1<<lcd_D6_bit);                        // repeat for each data bit
    if (theByte & 1<<6) lcd_D6_port |= (1<<lcd_D6_bit);

    lcd_D5_port &= ~(1<<lcd_D5_bit);
    if (theByte & 1<<5) lcd_D5_port |= (1<<lcd_D5_bit);

    lcd_D4_port &= ~(1<<lcd_D4_bit);
    if (theByte & 1<<4) lcd_D4_port |= (1<<lcd_D4_bit);

// write the data
                                                    // 'Address set-up time' (40 nS)
    lcd_E_port |= (1<<lcd_E_bit);                   // Enable pin high
    _delay_us(1);                                   // implement 'Data set-up time' (80 nS) and 'Enable pulse width' (230 nS)
    lcd_E_port &= ~(1<<lcd_E_bit);                  // Enable pin low
    _delay_us(1);                                   // implement 'Data hold time' (10 nS) and 'Enable cycle time' (500 nS)
}
void CopyStringtoLCD(const uint8_t *FlashLoc)
{
	uint8_t i;
	for(i=0;(uint8_t)pgm_read_byte(&FlashLoc[i]);i++)
	{
		LCDsendChar((uint8_t)pgm_read_byte(&FlashLoc[i]));
	}
}

void LCDsendChar(uint8_t ch)		//Sends Char to LCD
{

	//4 bit part
	PORTC=(ch&0b11110000);
	PORTD|=1<<lcd_RS_bit ;
	PORTG|=1<<lcd_E_bit;		
	_delay_ms(1);
	PORTG&=~(1<<lcd_E_bit);	
	PORTD&=~(1<<lcd_RS_bit);
	_delay_ms(1);
	PORTC=((ch&0b00001111)<<4);
	PORTD|=1<<lcd_RS_bit;
	PORTG|=1<<lcd_E_bit;		
	_delay_ms(1);
	PORTG&=~(1<<lcd_E_bit);	
	PORTD&=~(1<<lcd_RS_bit);
	_delay_ms(1);
}
