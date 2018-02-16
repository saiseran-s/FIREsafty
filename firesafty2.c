
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <compat/deprecated.h>
#define F_CPU 16000000

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



int main(void)
{
  
  char data[5];
  usart_init ();          /*initiate usart*/
  _delay_us(3000);
  UART_Tstring("AT\r\n");     /*check wifi response*/
  _delay_ms(2000);
  
  _delay_ms(2000);
  UART_Tstring("ATE0\r\n");
  dl;
  UART_Tstring("AT+CWMODE=1\r\n");/*Set mode*/      
  _delay_ms(3000);
  UART_Tstring("AT+CWJAP=\"Drago\",\"drago1234\"\r\n");/*connect to wifi*/
  _delay_ms(5000);
  dl;
  UART_Tstring("AT+CIPMUX=0\r\n");/*multiplexing*/
  _delay_ms(4000);
  
  while(1)
  {
    
    
    _delay_ms(4000);
    UART_Tstring("AT+CIPSTART=\"TCP\",\"acet.ac.in\",80\r\n");
    _delay_ms(6000);
    _delay_ms(5000);
    UART_Tstring("GET /firesafty/requestData/request.php?field=M1&field1=");
     uint8_t a= readAnalag(0);
    itoa(a,x,10);
    UART_Tstring(x);
    _delay_ms(2000);
    UART_Tstring("&field2=");
    _delay_us(2000);
    uint8_t b= readAnalag(3);
    float mv = ( b/1024.0)*5000; 
    int cel = mv/10;
    itoa(cel,y,10);
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
