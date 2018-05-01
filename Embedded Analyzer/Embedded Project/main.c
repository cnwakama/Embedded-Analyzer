/*
 * Embedproj.c
 *
 * Created: 4/24/2018 3:57:34 PM
 * Author : gdrivas and cnwakama
 */ 

//  Sample code for using USART, WINAVR
//
//  Resource: A. Kruger, April 2010
//
//  Transmission is polled, but reception is interrupt driven.  In
//  applications that interact with a user via a RS232 link and a PC,
//  polled data transmission from controller to PC is not a limitation.
//
//  The code below assumes an AVR with USART  TX and RX pins on PD0 and
//  PD1 respectively.  For example, ATmega88PA.  Change usart_init to
//  match your processor.

//#ifndef UART_RX0_BUFFER_SIZE
//#define UART_RX0_BUFFER_SIZE 128 /**< Size of the circular receive buffer, must be power of 2 */
//#endif

#define F_CPU 8000000L        // This should match the processor speed
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>     //  Routine for FLASH (program memory)
#include <util/delay.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#define BAUD_RATE 9600        // Baud rate. The usart_int routine
#define MAXLENGTH 8
#define Dev24C02  0x58      // device address of EEPROM 24C02, see datasheet
#define  COMMAND 0x00
#define UART_RX_BUFFER_MASK ( RX_BUFFER_SIZE - 1)
#define RX_BUFFER_SIZE 64
#define SAMPLE_RATE 6250 //Hz - cant read a frequency that is more than 3125
#include "Information.h"

// uses this and then sets 8N1

// Variables and #define for the RX ring buffer.


unsigned char rx_buffer[RX_BUFFER_SIZE];
volatile unsigned char rx_buffer_head;
volatile unsigned char rx_buffer_tail;



// Function prototypes.
unsigned char uart_buffer_empty(void);
void usart_prints(const char *ptr);
void usart_printf(const char *ptr);


void usart_putc(const char c); //look at size
void adc_init();
uint16_t adc_read(uint8_t ch);
void usart_init(void);
unsigned char usart_getc(void);

int uart_available(void);
void getSamples();

static uint16_t VOLTAGESAMPLE[FFT_N];
static complex_t bfly_buff[FFT_N];
static uint16_t spektrum[FFT_N/2];

static int counter = 0;
char str[35];


int main(void)
{
	
	sei();// Enable interrupts
	
	adc_init();
	usart_init();
	//i2c_init();
	//sprintf(q,"\n\r");
	
	
	/*char ho[25]; 
	sprintf(ho, "Hello World\n\r");
	usart_prints(ho);*/
	
	
	while (1){
		getSamples();
		//checkCommand();
	}
	
	
	return(1);
}
void printSummary(){
	int i = 0;
	float amplitude[10];
	float ptp[10];
	amplitude[i] = getAmpitude(VOLTAGESAMPLE);
	amplitude[i] = getConversion(amplitude[i]);
	ptp[0] = getPeaktoPeakVoltage(VOLTAGESAMPLE);
	float freq = 0.0;
	sprintf(str, "Analysis Summary - Sample\%d\n\r",counter);
	usart_prints(str);
	sprintf(str, "-----------------------------\n\r");
	usart_prints(str);
	sprintf(str, "Amplitude - %f V\n\r",amplitude[i]);
	usart_prints(str);
	sprintf(str, "Peak to Peak - %f V\n\r",ptp[i]);
	usart_prints(str);
	sprintf(str, "Frequency - %f Hz\n\r",freq);
	usart_prints(str);
	sprintf(str, "Intervals - %d:%d \n\r",0,100);
	usart_prints(str);
	sprintf(str, "Note - %s\n\r","A");
	usart_prints(str);
}
void getSamples(){
	uint8_t ch = 0;
	uint8_t intsize = (uint8_t) round((1/(SAMPLE_RATE)/FFT_N) * 1000000);
	
	for (uint8_t i = 0; i< FFT_N; i++){
		VOLTAGESAMPLE[i] = adc_read(ch) - 512;
		//VOLTAGESAMPLE[i] = VOLTAGESAMPLE[i] * 5/1024;
		//uint8_t val = i*time;
		
		///sprintf(str, "\n\rt=%d s, v=%f V",val,achf);
		// usart_prints(str);
		//delaying
		for (int j = 0; j<intsize; j++)
			_delay_us(1);
	}
}

void adc_init()
{
	// AREF = AVcc
	ADMUX = (1<<REFS0);
	
	// ADC Enable and pre-scaler of 128
	// 16000000/128 = 125000
	ADCSRA = (1<<ADEN|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0));
}
uint16_t adc_read(uint8_t ch)
{
	// select the corresponding channel 0~7
	// ANDing with Ì7? will always keep the value
	// of ÎchÌ between 0 and 7
	ch &= 0b00000111;  // AND operation with 7
	ADMUX = (ADMUX & 0xF8)|ch; // clears the bottom 3 bits before ORing
	
	// start single convention
	// write Ì1? to ADSC
	ADCSRA |= (1<<ADSC);
	
	// wait for conversion to complete
	// ADSC becomes Ì0? again
	// till then, run loop continuously
	while(ADCSRA & (1<<ADSC));
	
	return (ADCW);
}

/*************************************************************************
Function: uart_available()
Purpose:  Determine the number of bytes waiting in the receive buffer
Input:    None
Returns:  Integer number of bytes in the receive buffer
**************************************************************************/
int uart_available(void)
{
	return (UART_RX_BUFFER_MASK + rx_buffer_head - rx_buffer_tail) % UART_RX_BUFFER_MASK; /* uart_available */
}
void usart_init(void)
{
	// Configures the USART for serial 8N1 with
	// the Baud rate controlled by a #define.

	unsigned short s;
	
	// Set Baud rate, controlled with #define above.
	
	s = (double)F_CPU / (9600*16.0) - 1.0;
	UBRR0H = (s & 0xFF00);
	UBRR0L = (s & 0x00FF);

	// Receive complete interrupt enable: RXCIE0
	// Receiver & Transmitter enable: RXEN0,TXEN0

	UCSR0B = (1<<RXCIE0)|(1<<RXEN0)|(1<<TXEN0);

	// Along with UCSZ02 bit in UCSR0B, set 8 bits
	
	UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);
	
	DDRD |= (1<< 1);         // PD0 is output (TX)
	DDRD &= ~(1<< 0);        // PD1 is input (Rx)
	
	// Empty buffers
	
	rx_buffer_head = 0;
	rx_buffer_tail = 0;
}
void usart_printf(const char *ptr){

	// Send NULL-terminated data from FLASH.
	// Uses polling (and it blocks).

	uint8_t c;
	//was char

	while(pgm_read_byte_near(ptr)) {
		c = pgm_read_byte_near(ptr++);
		usart_putc(c);
	}
}
void usart_putc(const char c){

	// Send "c" via the USART.  Uses poling
	// (and it blocks). Wait for UDRE0 to become
	// set (=1), which indicates the UDR0 is empty
	// and can accept the next character.

	while (!(UCSR0A & (1<<UDRE0)))
	;
	UDR0 = c;
}
void usart_prints(const char *ptr){
	
	// Send NULL-terminated data from SRAM.
	// Uses polling (and it blocks).

	while(*ptr) {
		while (!( UCSR0A & (1<<UDRE0)))
		;
		UDR0 = *(ptr++);
	}
}
unsigned char usart_getc(void)
{

	// Get char from the receiver buffer.  This
	// function blocks until a character arrives.
	
	unsigned char c;
	
	// Wait for a character in the buffer.

	while (rx_buffer_tail == rx_buffer_head)
	;
	
	c = rx_buffer[rx_buffer_tail];
	if (rx_buffer_tail == RX_BUFFER_SIZE-1)
	rx_buffer_tail = 0;
	else
	rx_buffer_tail++;
	return c;
}
unsigned char uart_buffer_empty(void){
	// Returns TRUE if receive buffer is empty.
	
	return (rx_buffer_tail == rx_buffer_head);
}



ISR(USART_RX_vect)
{
	// UART receive interrupt handler.
	// To do: check and warn if buffer overflows.
	
	char c = UDR0;
	rx_buffer[rx_buffer_head] = c;
	if (rx_buffer_head == RX_BUFFER_SIZE - 1)
	rx_buffer_head = 0;
	else
	rx_buffer_head++;
}








