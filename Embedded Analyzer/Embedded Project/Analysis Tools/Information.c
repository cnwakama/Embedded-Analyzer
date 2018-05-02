/*
 * Information.c
 *
 * Created: 4/28/2018 12:03:22 PM
 *  Author: cnwakama
 */ 

#include "Information.h"



int16_t getAmpitude0(int16_t v[], int16_t num){
	struct pair p = {0,0,0,0}; 
	p = getMinMax(v,0,num);
	return(p.max);
}

int16_t getPeaktoPeakVoltage0(int16_t v[], int16_t num){
	struct pair p = getMinMax(v,0,num);
	return(p.max - p.min);
}

int16_t getAmpitude(int16_t v[]){
	struct pair p = {0,0,0,0};
	p = getMinMax(v,0,FFT_N);
	return(p.max);
}

int16_t getPeaktoPeakVoltage(int16_t v[]){
	struct pair p = getMinMax(v,0,FFT_N);
	return(p.max - p.min);
}

float getConversion(float v){
	 return(((float) 255.0/5.0)*(v));
}

int quantization (float v){
	return (int) round(v);
}

/************************************************************************/
/* Zero-Crossing Method                                                                     */
/************************************************************************/
float getfrequency(int v[FFT_N], uint16_t s ,int num){
	int checks[3];
	uint8_t c = s+1;
	checks[0] = s;
	checks[2] = FFT_N;  
	for (int i = 1; i<FFT_N; i++){
		if (abs(checks[0]) - v[i] < 2 )
			checks[c++] = i;
		if (c == 3)
			break; 
	}
	float ff = ((((float) checks[2]/FFT_N) * SAMPLE_RATE) - (((float) checks[0]/FFT_N) * SAMPLE_RATE));
	
	return ff;
}
/*void filterHarmonic(uint16_t harmonic[]){
	int fundfreq = harmonic[0];
}*/
void findMaxIntervals(uint16_t theshold, uint16_t harmonic[], uint16_t noise, uint16_t c, uint16_t * size){
	struct pair p = {0,0,0,0};
	
	int length = FFT_N/4;
	while (length < FFT_N/2){
		p = getMinMax(spektrum, noise , length);//find the max
		if (p.max > theshold)
			harmonic[c++] = p.imax;
		length += (p.imax - noise);
		noise = p.imax;
		(*size)++;
	}
	(*size)--;
}
void findHarmonic3(uint16_t theshold, uint16_t harmonic[], uint16_t noise, uint16_t *size){
	//looking at intervals for find the max
	//call a max function to return an index
	//then move the index to foundIndex + 1
	//if zero is returned as max move it to a new range
	//zero crossing
	findMaxIntervals(theshold, harmonic, noise, 0, size);
}
void findHarmonic2(uint16_t theshold, uint16_t harmonic[], uint16_t noise, uint16_t *size){
	//finding the max over the whole interval
	struct pair p = {0,0,0,0};
	p = getMinMax(spektrum, noise, FFT_N/2);//find the max
	harmonic[0] = p.imax;
	*size = 1;
	//could check the interval of max 
	//or use the filtering function to harmonics
	//findMaxIntervals(fft,  theshold, harmonic,  p.max + 1, 1);
	for (int i = p.imax + 1; i < FFT_N/2; i++){
		if (spektrum[i] > theshold)
			harmonic[(*size)++] = i;
	}
	(*size)--;
}
/************************************************************************/
/* Get the fund freq maybe need to add some type of max or maximum for better accuracy                                                                     */
/************************************************************************/
//looking at the values that are higher than theshold using the noise variable to get rid of initial reading 
void findHarmonic1(uint16_t theshold, uint16_t harmonic[], uint16_t noise, uint16_t *size){
	//find maximum then look at range 1 from both range of left and right 
	*size =0;
	for (int i = noise; i<FFT_N/2 + 1; i++){
		if (spektrum[i] > theshold)
			harmonic[(*size)++] = i;
		//return ((fft[i]/FFT_N) * SAMPLE_RATE);
	}
	(*size)--;
	/*return ((fft[0]/FFT_N) * SAMPLE_RATE);
	for (int i = 0; i<FFT_N/2 + 1; i++){
		if (fft[i] > theshold)
			return ((fft[i]/FFT_N) * SAMPLE_RATE);
	}
	return ((fft[0]/FFT_N) * SAMPLE_RATE);*/
	
}
void getfft(int v[FFT_N]){
	fft_input(v, bfly_buff);
	fft_execute(bfly_buff);
	fft_output(bfly_buff, spektrum);
	
}
/************************************************************************/
/*   Uses fft (spektrum) to get a frequency                                                                    */
/************************************************************************/
float getfrequencyfft(uint16_t s [FFT_N/2], uint16_t f){
	return (( (float) s[f]/FFT_N) * 1.0 * SAMPLE_RATE);
}


