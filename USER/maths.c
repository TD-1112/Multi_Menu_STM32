#include "maths.h"

float Abs(float num)        
{ 
	return num >= 0 ? num : -num;
}
float Max(float m, float n) 
{ 
	return m > n ? m : n;
}
float Min(float m, float n) 
{ 
	return m < n ? m : n;
}
