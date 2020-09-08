//Note
/*
	Simple C++ Implementation of Ken Perlins, Perlin Noise algorithm
	Copyright Ken Perlin 2002.

	ref: https://github.com/Reputeless/PerlinNoise/blob/master/PerlinNoise.hpp
*/
#pragma once
#include "System/Types.h"

class PerlinNoise
{
private:
	Int16 p[512] = { 0 }; //Pseudo random table

public:
	PerlinNoise();
	PerlinNoise(unsigned int seed);
	~PerlinNoise();

public:
	float Noise(float x, float y = 0.0f, float z = 0.0f);
	float OctaveNoise(int octave, float x, float y = 0.0f, float z = 0.0f);

private:
	float Grad(unsigned int hash, float x, float y, float z = 0);
	void GeneratePermutations(unsigned int seed);
};