/*
 * Information.h
 *
 * Created: 4/28/2018 2:43:16 PM
 *  Author: cnwakama
 */ 


#ifndef INFORMATION_H_
#define INFORMATION_H_


#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>     //  Routine for FLASH (program memory)
#include <util/delay.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "ffft.h"

#include "pair.h"

//#define SIZE 1250
#define SAMPLE_RATE 4807 //Hz - cant read a frequency that is more than 2403 Hz

static complex_t bfly_buff[FFT_N];
static uint16_t spektrum[FFT_N/2];

extern int16_t getAmpitude0(int16_t v[], int16_t num);
extern int16_t getPeaktoPeakVoltage0(int16_t v[], int16_t num);
extern int16_t getAmpitude(int16_t v[]);
extern int16_t getPeaktoPeakVoltage(int16_t v[]);
extern float getConversion(float v);
extern int16_t quantization (float v);
extern float getfrequency(int v[FFT_N], uint16_t s ,int num);
extern void getfft(int v[FFT_N]);
extern void findMaxIntervals(uint16_t fft[FFT_N/2], int theshold, uint16_t harmonic[], int noise, int c);
extern void findHarmonic3(uint16_t fft[FFT_N/2], int theshold, uint16_t harmonic[], int noise);
extern void findHarmonic2(uint16_t fft[FFT_N/2], int theshold, uint16_t harmonic[], int noise);
extern void findHarmonic1(uint16_t fft[FFT_N/2], int theshold, uint16_t harmonic[], int noise);
//
//
#endif INFORMATION_H_ 