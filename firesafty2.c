/*
AVR ATmega8
Description : This code is based on ATmega8 and integreated with WIFI esp8266, mq135, flame sensor and lm35.
Author    :- S Sai Seran
Co-Author :-M Pavan Sai
*/
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <compat/deprecated.h>

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

unsigned char x[32],y[32],z[32];
unsigned char GET[57]="GET /firesafty/requestData/request.php?field=M1&field1=",f2[9]="&field2=",f3[9]="&field3=";

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
  
  UART_Tstring("AT+CIPSTART=\"TCP\",\"acet.ac.in\",80\r\n");
  _delay_ms(4000);
   UART_Tstring("AT+CIPSEND=87\r\n");
  _delay_ms(3000);
  UART_Tstring(GET);
  _delay_us(100);

  uint8_t a = readAnalag(0);
  float mv = {(a/1024.0)*5000};
  uint16_t cel_i = mv/10;
  uint16_t cel_d = [(mv*10.0) - (cel_i*100)];
  itoa(cel_i,x,10);
  UART_Tstring(x);
  UART_Tstring(".");
  itoa(cel_d,x,10);
  UART_Tstring(x);
  _delay_ms(200);


  UART_Tstring(f2);
  _delay_us(200);
  uint16_t b = readAnalag(3);
  itoa(b,y,10);
  _delay_us(200);
  UART_Tstring(y);
  _delay_us(100);
  UART_Tstring(f3);
  _delay_us(200);
  uint16_t c = readAnalag(5);
  itoa(c,z,10);
  UART_Tstring(z);
  UART_Tstring("\r\n");
  _delay_ms(4000);
  UART_Tstring("AT+CIPCLOSE");
  _delay_ms(2000);

  }
}