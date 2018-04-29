/*
 * pair.h
 *
 * Created: 4/28/2018 2:53:13 PM
 *  Author: cnwakama
 */ 


#ifndef PAIR_H_
#define PAIR_H_

struct pair
{
	float min;
	float max;
};

extern struct pair getMinMax(float arr[], int n);



#endif /* PAIR_H_ */