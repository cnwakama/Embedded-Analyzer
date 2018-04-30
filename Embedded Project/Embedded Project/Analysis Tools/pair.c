/*
 * pair.c
 *
 * Created: 4/28/2018 1:03:48 PM
 *  Author: cnwakama
 * From https://www.geeksforgeeks.org/maximum-and-minimum-in-an-array/
 */ 

#include "pair.h"

 
struct pair getMinMax(float arr[], int n)
{
  struct pair minmax;     
  int i;
   
  /*If there is only one element then return it as min and max both*/
  if (n == 1)
  {
     minmax.max = arr[0];
     minmax.min = arr[0];     
     return minmax;
  }    
 
  /* If there are more than one elements, then initialize min 
      and max*/
  if (arr[0] > arr[1])  
  {
      minmax.max = arr[0];
      minmax.min = arr[1];
  }  
  else
  {
      minmax.max = arr[1];
      minmax.min = arr[0];
  }    
 
  for (i = 2; i<n; i++)
  {
    if (arr[i] >  minmax.max)      
      minmax.max = arr[i];
   
    else if (arr[i] <  minmax.min)      
      minmax.min = arr[i];
  }
   
  return minmax;
}