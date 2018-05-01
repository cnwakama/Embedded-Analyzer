/*
 * pair.c
 *
 * Created: 4/28/2018 1:03:48 PM
 *  Author: cnwakama
 * From https://www.geeksforgeeks.org/maximum-and-minimum-in-an-array/
 */ 

#include "pair.h"

 
struct pair getMinMax(float arr[], int s, int n)
{
  struct pair minmax;     
  int i;
   
  /*If there is only one element then return it as min and max both*/
  if (n == 1)
  {
     minmax.max = arr[0];
     minmax.min = arr[0];
	 minmax.imax = 0;
	 minmax.imin = 0;     
     return minmax;
  }    
 
  /* If there are more than one elements, then initialize min 
      and max*/
  if (arr[s] > arr[s+1])  
  {
      minmax.max = arr[s];
      minmax.min = arr[s+1];
	  minmax.imin = s+1;
	  minmax.imax = s;
  }  
  else
  {
      minmax.max = arr[s+1];
      minmax.min = arr[s];
	  minmax.imin = s+1;
	  minmax.imax = s;
  }    
 s = s+2;
  for (i = s; i<n; i++)
  {
    if (arr[i] >  minmax.max)  {    
      minmax.max = arr[i];
	  minmax.imax = i;
	}
   
    else if (arr[i] <  minmax.min) {     
      minmax.min = arr[i];
	  minmax.min = i;
	}
  }
   
  return minmax;
}