/*
created by sai seran
*/
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <string.h>
#include <avr/interrupt.h>
#include <stdio.h>

#define F_CPU 16000000
#define BAUD 115200
#define MYUBRR F_CPU/16/BAUD-1

//int usart_putchar_printf(char  var, FILE *stream);
//static FILE mystdout = FDEV_SETUP_STREAM(usart_putchar_printf, NULL, _FDEV_SETUP_WRITE);






#define dl _delay_ms(3000)

unsigned char x[32],y[32],z[32];

void usart_init() {
	// Set baud rate
	//UBRRH = (uint8_t)(ubrr>>8);
	UBRRL = 8;
	// Enable receiver and transmitter
	UCSRB = (1<<RXEN)|(1<<TXEN);
	// Set frame format: 8data, 1stop bit
	UCSRC = (1<<URSEL)|(3<<UCSZ0);
}


void UART_Tchar(unsigned char data)
{
	UDR = data;
	while(!(UCSRA & (_BV(TXC)))); //CHECK FLAG TXC
	
	
}
void UART_Tstring(char *p)
{
	while(*p){
		UART_Tchar(*(p++));
		_delay_us(100);
	}
}

char RxChar (void)
{
	
	  while(!(UCSRA&(1<<RXC)));
	 return(UDR);
}
// char* Rstring()
// {
// 	//  char a[128],*ret;
// 	//  int i=0;
// 	//  while(a[i++]=RxChar()!=0x0D);
// 	//  a[i]="\0";
// 	//  ret =a;
// 	//  return(ret);
// }
// int usart_putchar_printf(char var, FILE *stream) {
// 	// translate \n to \r for br@y++ terminal
// 	if (var == '\n') UART_Tstring('\r');
// 	UART_Tstring(var);
// 	return 0;
// }
int main(void)
{
	DDRC=0xEA;
	DDRC|=0x48;
	
	//stdout = &mystdout;
	usart_init ();
	PINC=(1<<0) |(1<<2) | (1<<5);
	uint8_t	a=bit_is_clear(PINC,0),b=bit_is_clear(PINC,2),c=bit_is_clear(PINC,5);
	//printf("Here is myvalue as a char: %c\n ", a);
	//UART_init();
	
	_delay_us(3000);
	UART_Tstring("AT\r\n");
	_delay_ms(2000);
	char d[32];
	
	_delay_ms(2000);
	UART_Tstring("ATE0\r\n");
	dl;
	UART_Tstring("AT+CWMODE=1\r\n");
	_delay_ms(3000);
	UART_Tstring("AT+CWJAP=\"Drago\",\"drago1234\"\r\n");
	_delay_ms(5000);
	dl;
	UART_Tstring("AT+CIPMUX=1\r\n");
	_delay_ms(4000);
	while(1)
	{
		_delay_ms(3000);
		UART_Tstring("AT+CIPSERVER=1,80\r\n");
		_delay_ms(4000);
		UART_Tstring("AT+CIPSTART=4,\"TCP\",\"api.thingspeak.com\",80\r\n");
		_delay_ms(6000);
		UART_Tstring("AT+CIPSEND=4,128\r\n");
		_delay_ms(5000);
		UART_Tstring("GET /update?api_key=VAT0C7IVVWJFDCD1&field1=");
		_delay_us(2000);
		itoa(a,x,10);
		_delay_us(2000);
		a++;
		UART_Tstring(x);
		_delay_ms(2000);
		UART_Tstring("&field2=");
		_delay_us(2000);
		
		itoa(b,y,10);
		_delay_us(2000);
		UART_Tstring(y);
		b++;
		UART_Tstring("&field3=");
		_delay_us(2000);
		
		itoa(c,z,10);
		c++;
		UART_Tstring(z);
		_delay_ms(3000);
		UART_Tstring("\r\n");
		_delay_ms(9000);
		
	}
}

