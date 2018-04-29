/*
 * Information.c
 *
 * Created: 4/28/2018 12:03:22 PM
 *  Author: cnwakama
 */ 

#include "Information.h"



int getAmpitude0(float v[], int num){
	struct pair p = {0.0}; 
	p = getMinMax(v,num);
	return(p.max);
}

int getPeaktoPeakVoltage0(float v[], int num){
	struct pair p = getMinMax(v,num);
	return(p.max - p.min);
}

int getAmpitude(float v[]){
	struct pair p = {0.0};
	p = getMinMax(v,SIZE);
	return(p.max);
}

int getPeaktoPeakVoltage(float v[]){
	struct pair p = getMinMax(v,SIZE);
	return(p.max - p.min);
}

float getConversion(float v){
	 return((255.0/5.0)*(v));
}

int quantization (float v){
	return (int) round(v);
}

int getfrequency(int v[], int num){
	int copySample[SIZE]; 
	return 0;
}

