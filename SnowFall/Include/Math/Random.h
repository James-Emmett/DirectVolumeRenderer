//Note:
/*
	The core implmentation is based on George Marsaglia Xorshift,
	However This is xorwow a slight variation using addivitve counter
	modulo 2^32

	//https://v8.dev/blog/math-random?showComment=1450389868643#c2004131565745698275
	//https://xor0110.wordpress.com/2010/09/24/how-to-generate-floating-point-random-numbers-efficiently/

	All Ranges are inclusive.
*/
#pragma once
#include "Vector2.h"
#include "Vector3.h"

class Random
{
private:
	unsigned int mx, my, mz, mw, mc;

public:
	Random();
	Random(unsigned int seed);
	~Random();

public:
	float Next();//0.0f - 1.0f
	int   Range(int min, int max);
	float Range(float min, float max);
	Vector2 PointInCircle(float radius = 1.0f);
	Vector2 PointOnCircle(float radius = 1.0f);
	Vector3 PointInSphere(float radius = 1.0f);
	Vector3 PointOnSphere(float radius = 1.0f);

private:
	unsigned int Shift32(unsigned int x);
	unsigned int xorwow();
};