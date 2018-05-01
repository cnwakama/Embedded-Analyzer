/*
 * Information.c
 *
 * Created: 4/28/2018 12:03:22 PM
 *  Author: cnwakama
 */ 

#include "Information.h"



float getAmpitude0(float v[], int num){
	struct pair p = {0.0}; 
	p = getMinMax(v,num);
	return(p.max);
}

float getPeaktoPeakVoltage0(float v[], int num){
	struct pair p = getMinMax(v,num);
	return(p.max - p.min);
}

float getAmpitude(float v[]){
	struct pair p = {0.0};
	p = getMinMax(v,FFT_N);
	return(p.max);
}

float getPeaktoPeakVoltage(float v[]){
	struct pair p = getMinMax(v,FFT_N);
	return(p.max - p.min);
}

float getConversion(float v){
	 return((255.0/5.0)*(v));
}

int quantization (float v){
	return (int) round(v);
}

float getfrequency(float v[], int num){
	int copySample[FFT_N]; 
	return 0;
}
float getfrequency0(float v[]){
	int copySample[FFT_N];
	return 0;
}

