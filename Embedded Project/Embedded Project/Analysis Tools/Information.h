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

#include "pair.h"
#define SIZE 1250

extern float getAmpitude0(float v[], int num);
extern float getPeaktoPeakVoltage0(float v[], int num);
extern float getfrequency0(float v[], int num);
extern float getAmpitude(float v[]);
extern float getPeaktoPeakVoltage(float v[]);
extern float getConversion(float v);
extern int quantization (float v);
extern float getfrequency(float v[]);

#endif /* INFORMATION_H_ */