//--------------------------------------------------------------------------------------
//Author: David Greenberg
//DESC: Generalized utility class containing functions utilized across multiple classes
//
//--------------------------------------------------------------------------------------
#pragma once
#include <stdlib.h>
#include <math.h>
#define CLASS_DEFINED_UTIL

using namespace std;

//*******************************************
// Random Number Functions
//*******************************************

//Returns a random int between x and y
inline int RandInt(int x, int y){
	return rand() % (y - x + 1) + x;
}

//return a random float between 0 and 1
inline double RandFloat() {
	return (rand()) / (RAND_MAX + 1.0);
}


//returns a random bool
inline bool RandBool()
{
	if (RandInt(0, 1)) return true;

	else return false;
}

//Returns a random float in the range -1 < n < 1
inline double RandomClamped(){
	return RandFloat() - RandFloat();
}

//return a random float between a and b
inline double RandomClampedBetween(double a, double b){
	if (b < a){
		double c = b;
		b = a;
		a = c;
	}
	double random = ((double)rand()) / (double)RAND_MAX;
	double diff = b - a;
	double r = random * diff;
	return a + r;
}
