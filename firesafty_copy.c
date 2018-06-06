#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <compat/deprecated.h>
#define F_CPU 16000000
uint8_t c=0,I_RH,D_RH,I_Temp,D_Temp,CheckSum;
#define DHT_PIN 5
void Request()
{
	DDRD  |=(1<<DHT_PIN);
	PORTD &~(1<<DHT_PIN);
	_delay_ms(20);
	PORTD |=(1<<DHT_PIN);
}

void Response()
{
	DDRD &~(1<<DHT_PIN);
	while(PIND & (1<<DHT_PIN));
	while((PIND&(1<<DHT_PIN))==0);
	while(PIND&(1<<DHT_PIN));
}
int8_t Receive_data()
{
	for(int q=0;q<8;q++)
	{
		while((PIND & (1<<5))==0);
		_delay_us(30);
		if(PIND&(1<<5))
		c=(c<<1)|(0x01);
		else
		c =(c<<1);
		while(PIND&(1<<5));
	}
	return c;
	
}



uint16_t readAnalag(uint8_t num)
{
	ADMUX = num;                   // select channel
	sbi(ADMUX, REFS0);             // use Vcc as reference voltage
	sbi(ADCSRA, ADPS0);            // divide clock for accurate reading
	sbi(ADCSRA, ADPS1);            // divide clock for accurate reading
	sbi(ADCSRA, ADPS2);            // divide clock for accurate reading
	sbi(ADCSRA, ADEN);             // enable converter
	sbi(ADCSRA, ADSC);             // start conversion
	while ((ADCSRA & 0x40) > 0);   // wait until conversion is complete
	uint8_t lowByte = ADCL;
	uint8_t highByte = ADCH;
	cbi(ADCSRA, ADEN);             // disable converter
	return ((highByte << 8) + lowByte);
}
#define dl _delay_ms(3000)

char x[32],y[32],z[32];

void usart_init() {
	// Set baud rate
	UBRRL = 8;
	// Enable receiver and transmitter
	UCSRB = (1<<RXEN)|(1<<TXEN);
	// Set frame format: 8data, 1stop bit
	UCSRC = (1<<URSEL)|(3<<UCSZ0);
}


void UART_Tchar(unsigned char data)
{
	UDR = data;
	while(!(UCSRA & (_BV(TXC))));      //CHECK FLAG TXC
	
	
}
void UART_Tstring(char *p)
{
	while(*p){
		UART_Tchar(*(p++));
		_delay_us(100);
	}
}

/*
char RxChar (void)
{
	
	while(!(UCSRA&(1<<RXC)));
	return(UDR);
}*/

int main(void)
{
	char data[5];
//------------------------------------------------------------------------
/*
	Request();						/ *send request pulse* /
	Response();						/ *receive response* /
	I_RH=Receive_data();			/ *store first 8bit in I_RH* /
	D_RH=Receive_data();			/ *store next 8bit in D_RH* /
	I_Temp=Receive_data();			/ *store next 8bit in I_Temp* /
	D_Temp=Receive_data();			/ *store next 8bit in D_Temp* /
	CheckSum=Receive_data();		/ *store next 8bit in CheckSum* /
*/
//-------------------------------------------------------------------------
	usart_init ();					/*initiate usart*/
	_delay_us(3000);
	UART_Tstring("AT\r\n");			/*check wifi response*/
	_delay_ms(2000);
	_delay_ms(2000);
	UART_Tstring("ATE0\r\n");
	dl;
	UART_Tstring("AT+CWMODE=1\r\n");/*Set mode*/			
	_delay_ms(3000);
	UART_Tstring("AT+CWJAP=\"MADDULA\",\"Pavan@9949\"\r\n");/*connect to wifi*/
	_delay_ms(5000);
	dl;
	UART_Tstring("AT+CIPMUX=1\r\n");/*multiplexing*/
	_delay_ms(4000);
	
	while(1)
	{
		
		PORTB=0x00;
		PINB=(1<<4);
		
		/*unsigned int a =readAnalag(1);
		uint8_t b=45;
		uint8_t c=55;*/
			Request();						/*send request pulse*/
			Response();						/*receive response*/
			I_RH=Receive_data();			/*store first 8bit in I_RH*/
			D_RH=Receive_data();			/*store next 8bit in D_RH*/
			I_Temp=Receive_data();			/*store next 8bit in I_Temp*/
			D_Temp=Receive_data();			/*store next 8bit in D_Temp*/
			CheckSum=Receive_data();		/*store next 8bit in CheckSum*/

		_delay_ms(3000);
		UART_Tstring("AT+CIPSERVER=1,80\r\n");
		_delay_ms(4000);
		UART_Tstring("AT+CIPSTART=4,\"TCP\",\"api.thingspeak.com\",80\r\n");
		_delay_ms(6000);
		UART_Tstring("AT+CIPSEND=4,128\r\n");
		_delay_ms(5000);
		UART_Tstring("GET /update?api_key=VAT0C7IVVWJFDCD1&field1=");
		if ((I_RH+D_RH+I_Temp+D_Temp)!=CheckSum)
		{
			UART_Tstring("error");
		}
		else
		{
			itoa(I_Temp,data,10);
			UART_Tstring(data);
			UART_Tstring(".");
			itoa(D_Temp,data,10);
			UART_Tstring(data);
		}
		_delay_ms(2000);
		UART_Tstring("field2=");
		_delay_us(2000);
		uint8_t b= readAnalag(3);
		itoa(b,y,10);
		UART_Tstring(y);
		UART_Tstring("&field3=");
		_delay_us(2000);
		uint8_t c=readAnalag(5);
		itoa(c,z,10);
		UART_Tstring(z);
		UART_Tstring("\r\n");
		_delay_ms(9000);
	}
}
