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
#define SAMPLE_RATE 4807 //Hz - cant read a frequency that is more than 2403 Hz
#include "Information.h"

// uses this and then sets 8N1

// Variables and #define for the RX ring buffer.


unsigned char rx_buffer[RX_BUFFER_SIZE];
volatile unsigned char rx_buffer_head;
volatile unsigned char rx_buffer_tail;



// Function prototypes.
unsigned char uart_buffer_empty(void);
void usart_init(void);
void usart_prints(const char *ptr);
void usart_printf(const char *ptr);
void usart_putc(const char c); //look at size
unsigned char usart_getc(void);
int uart_available(void);

void adc_init();
uint16_t adc_read(uint8_t ch);
void getSamples();
void displayfreqharmonics();
void displayWaveform();
void displaySpectrum();
void printHelloWorld();
void printSummary();
void analysis();


static int16_t counter = 1;
static int16_t VOLTAGESAMPLE[FFT_N];
static const uint16_t NOISE = 4;
static const uint16_t VOLTAGENOISE = 2;
static float amplitude;
static float ptp;
static float frequencies;
static float harmonics[FFT_N/4];
static uint16_t numberOfHarmonics = 0;
char str[45];


int main(void)
{
	
	sei();// Enable interrupts
	
	adc_init();
	usart_init();
	//i2c_init();
	//sprintf(q,"\n\r");
	
	
	//char ho[25]; 
	//sprintf(ho, "Hello World\n\r");
	//usart_prints(ho);
	printHelloWorld();
	
	
	while (1){
		//getSamples();
		//analysis();
		//printSummary();
		//printHelloWorld();
		//printSummary();
		//checkCommand();
	}
	
	
	return(1);
}
void analysis(){
	//amplitude 
	//peak-to-peak
	//fundamental frequency
	//harmonic frequency
	//int i = 0;
	amplitude = getAmpitude(VOLTAGESAMPLE);
	amplitude = getConversion(amplitude);
	ptp = getPeaktoPeakVoltage(VOLTAGESAMPLE);
	ptp = getConversion(ptp);
	frequencies = getfrequency(VOLTAGESAMPLE, VOLTAGENOISE, FFT_N); //Zero-Crossing Method
	getfft(VOLTAGESAMPLE);
	//findHarmonic1(2, harmonics, NOISE);
	findHarmonic2(2, harmonics, NOISE, &numberOfHarmonics);
	//findHarmonic3(2, harmonics, NOISE);
	
}
void printHelloWorld(){
	sprintf(str, "Hello World");
	usart_prints(str);
}
void printSummary(){
	sprintf(str, "Analysis Summary of Signal Sample \%d\n\r",counter++);
	usart_prints(str);
	sprintf(str, "-----------------------------\n\r");
	usart_prints(str);
	displayWaveform();
	sprintf(str, "\t");
	usart_prints(str);
	sprintf(str, "Waveform\n\r");
	usart_prints(str);
	displaySpectrum();
	sprintf(str, "\t");
	usart_prints(str);
	sprintf(str, "Spectrum\n\r");
	usart_prints(str);
	sprintf(str, "Amplitude - %f V\n\r",amplitude);
	usart_prints(str);
	sprintf(str, "Peak to Peak - %f V\n\r",ptp);
	usart_prints(str);
	sprintf(str, "Fundamental Frequency (Zero-Crossing) - %f Hz\n\r",frequencies);
	usart_prints(str);
	sprintf(str, "Fundamental Frequency (FFT) - %f Hz\n\r", getfrequencyfft(spektrum,harmonics[0]));
	usart_prints(str);
	sprintf(str, "Harmonic Frequencies");
	usart_prints(str);
	sprintf(str, "Number of Harmonics - %d\n\r", numberOfHarmonics);
	usart_prints(str);
	displayfreqharmonics();
	//display frequencies of harmonics
	//sprintf(str, "Note - %s\n\r","A");
	//usart_prints(str);
}
void displayfreqharmonics(){
	for (int i = 1; i <= numberOfHarmonics; i++){
		sprintf(str, "Harmonic %d\t", i);
		usart_prints(str);
	}
	sprintf(str, "\n\r");
	usart_prints(str);
	for (int i = 1; i <= numberOfHarmonics; i++){
		sprintf(str, "-------- \t");
		usart_prints(str);
	}
	for (int i = 1; i <= numberOfHarmonics; i++){
		sprintf(str, "Frequency %f Hz\t",  getfrequencyfft(spektrum,harmonics[i]));
		usart_prints(str);
	}
	//display phases maybe
	//display a view of the frequency for each harmonics
	
}
void displaySpectrum(){
	uint16_t m, n, s;
	for (n = 0; n < FFT_N / 2; n++) {
		s = spektrum[n];
		xmitf(PSTR("\r\n%4u:%5u "), n, s);
		s /= (FFT_N * 2);
		for (m = 0; m < s; m++) xmit('*');
	}
}
void displayWaveform(){
	uint16_t m, n, s;
	for (n = 0; n < FFT_N; n++) {
		s = VOLTAGESAMPLE[n];
		xmitf(PSTR("\r\n%4u:%6d "), n, s);
		s = (s + 512) / (FFT_N *4); //1024 is the size of the sample and 32768 is the revoluation at 16 bits but mine is 10 not 16
		for (m = 0; m < s; m++) xmit(' ');
		xmit('*');
	}
}
void getSamples(){
	uint8_t ch = 0;
	uint8_t intsize = (uint8_t) round((1/(SAMPLE_RATE)/FFT_N) * 1000000);
	
	for (uint8_t i = 0; i< FFT_N; i++){
		VOLTAGESAMPLE[i] = adc_read(ch) - 512; //scaling it back down to used in the fft
		
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








