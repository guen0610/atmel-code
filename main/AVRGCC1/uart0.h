/**********************************************
Filename 	: uart0.h
Target   	: Atmega 128
CPU Freq 	: 16Mhz
Description 	: Library of UART0 functions
Author    	: Gantumur
***********************************************/
#include<avr/io.h>

//#include <inttype.h>
#define USART_BAUDRATE 115200   
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

void uart0_init(uint32_t baud);										
void UART0_Transmit_char(char char_data);
char UART0_Receive_char(void);
void UART0_Transmit_num(unsigned int num_data);
void UART0_Transmit_string(char *str_data);
unsigned int UART0_Receive_num(void);
void UART0_Receive_string(char *str);
char UReadData0(void);
void UReadBuffer0(void *buff,uint16_t len);
void UFlushBuffer0();
uint8_t UDataAvailable0();


#define RECEIVE_BUFF_SIZE0 64
//Varriables
volatile char URBuff0[RECEIVE_BUFF_SIZE0];	//USART Receive Buffer
volatile int8_t UQFront0;
volatile int8_t UQEnd0;

void uart0_init(uint32_t baud)
{ 
 UQFront0=UQEnd0=-1;
if(baud==4800)
{
    UBRR0H = 0;
	UBRR0L = 207;                
}	
else if(baud==9600)
{
    UBRR0H = 0;
	UBRR0L = 103; 
}
else if(baud==14400)
{
    UBRR0H = 0;
	UBRR0L = 68; 
}
else if(baud==19200)
{
    UBRR0H = 0;
	UBRR0L = 51; 
}
else if(baud==38400)
{
    UBRR0H = 0;
	UBRR0L = 25; 
}
else if(baud==57600)
{
    UBRR0H = 0;
	UBRR0L = 16; 
}
else if(baud==115200)
{
    UBRR0H = 0;
	UBRR0L = 8;                
}
else 
{

    UBRR0H = 0;
	UBRR0L = 8;
}

	UCSR0A=0X0;
	UCSR0B=0X98;
	UCSR0C=0X06;


	UCSR0B=(1<<RXCIE0)|(1<<RXEN0)|(1<<TXEN0);
}



void UART0_Transmit_char(char char_data)		/*Function to transmit character data*/
{
	while((UCSR0A & 0X20)!=0X20); 			/* wait till data register is empty*/
	UDR0=char_data;
}


char UART0_Receive_char(void)				/*Function to receive character data*/
{
	while((UCSR0A & 0X80)!=0X80);			/*wait till reciption is over*/
	return UDR0;
}


void UART0_Transmit_num(unsigned int num_data)/*Function to transmit numeric data*/
{
	int a[20];
	int i=0;
	if(num_data==0) UART0_Transmit_char('0');
	while(num_data>0)
   {
        a[i]=(num_data%10) | 0x30;			/* separating digits of num, storing it into array,
											OR it with 0x30 to convert it into ASCII character */
        num_data=num_data/10;
        i++;
   }
   i=i-1;
	while(i>=0)
	{
		UART0_Transmit_char((char)a[i]);	/*Type cast a[i] into character and pass it into transmit_char funcion*/
		i--;
	}
}


void UART0_Transmit_string(char *str_data)	/*Function to transmit string*/
{
	while(*str_data!='\0')
	{
	UART0_Transmit_char(*str_data);
	str_data++;
	}
}

unsigned int UART0_Receive_num(void)		 /*Function to receive numeric data*/

{
	/* for receiving numeric data, first take one by one characters into an array,
	convert it from ASCII to numeric value by AND with 0x0f,,
	and make a complete number from its digits*/
	unsigned int b[5];
	char t;

	int x=1;
	int k=0;
	unsigned int num=0;
	while((t=UART0_Receive_char())!=13)				
	{
		
	b[k] = (unsigned int)t & 0x000F;
	k++;
	}
	
	k=k-1;
	while(k>=0)
	{
	num=num+(b[k]*x);
	x=x*10;
	k--;
	}
	return num;
}

void UART0_Receive_string(char *str)			/*Function to receive a string*/
{
    
        char c;
        while(c!=13)
		{
            c=UART0_Receive_char();
			if(c!=13)
			{
            *str=c;
            str++;
			}
        }
    return;
    
}
uint8_t UDataAvailable0()
{
	if(UQFront0==-1) return 0;
	if(UQFront0<UQEnd0)
		return(UQEnd0-UQFront0+1);
	else if(UQFront0>UQEnd0)
		return (RECEIVE_BUFF_SIZE0-UQFront0+UQEnd0+1);
	else
		return 1;
}
void UFlushBuffer0()
{
	while(UDataAvailable0()>0)
	{
		UReadData0();
	}
}
char UReadData0()
{
	char data;
	
	//Check if q is empty
	if(UQFront0==-1)
		return 0;
	
	data=URBuff0[UQFront0];
	
	if(UQFront0==UQEnd0)
	{
	//If single data is left
	//So empty q
	UQFront0=UQEnd0=-1;
	}
	else
	{
		UQFront0++;

		if(UQFront0==RECEIVE_BUFF_SIZE0)
		UQFront0=0;
	}

	return data;
}
