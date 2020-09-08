#include "Math/PerlinNoise.h"
#include "Math/Mathf.h"
#include <algorithm>
#include <random>

//---------------------------------CONSTRUCTOR---------------------------------
// Starts the Random permutation table with a seed.
//-----------------------------------------------------------------------------
PerlinNoise::PerlinNoise()
{
	GeneratePermutations(std::default_random_engine::default_seed);
}

//---------------------------------CONSTRUCTOR---------------------------------
// Starts the Random permutation table with a seed.
//-----------------------------------------------------------------------------
PerlinNoise::PerlinNoise(unsigned int seed)
{
	GeneratePermutations(seed);
}

//----------------------------------DESTRUCTOR---------------------------------
//	Default Constructor
//-----------------------------------------------------------------------------
PerlinNoise::~PerlinNoise()
{
}

//-----------------------------------PERLIN_NOISE------------------------------
//  Returns perlin noise between 0 and 1.
//-----------------------------------------------------------------------------
float PerlinNoise::Noise(float x, float y, float z)
{
	//Find Unit Cube that contains the Point.
	const int X = (int)Mathf::Floor(x) & 255;
	const int Y = (int)Mathf::Floor(y) & 255;
	const int Z = (int)Mathf::Floor(z) & 255;

	//Get x,y,z of Relative to cube, Fractional part:
	// 1.56 == 0.56 etc.
	x -= Mathf::Floor(x);
	y -= Mathf::Floor(y);
	z -= Mathf::Floor(z);

	//Smoothstep each point, prevents linear stepping of values.
	const float u = Mathf::SmoothStepKP(0, 1, x);
	const float v = Mathf::SmoothStepKP(0, 1, y);
	const float w = Mathf::SmoothStepKP(0, 1, z);

	//Get Hash of each Corner of the Cube.
	const int A = p[X] + Y;
	const int AA = p[A] + Z;
	const int AB = p[A + 1] + Z;
	const int B = p[X + 1] + Y;
	const int BA = p[B] + Z;
	const int BB = p[B + 1] + Z;

	//Blend the results from the 8 corners of the cube.
	float res = Mathf::Lerp(w, Mathf::Lerp(v, Mathf::Lerp(u, Grad(p[AA], x, y, z), Grad(p[BA], x - 1, y, z)),
		Mathf::Lerp(u, Grad(p[AB], x, y - 1, z), Grad(p[BB], x - 1, y - 1, z))),
		Mathf::Lerp(v, Mathf::Lerp(u, Grad(p[AA + 1], x, y, z - 1), Grad(p[BA + 1], x - 1, y, z - 1)),
			Mathf::Lerp(u, Grad(p[AB + 1], x, y - 1, z - 1), Grad(p[BB + 1], x - 1, y - 1, z - 1))));

	return res * 0.5f + 0.5f;
}

float PerlinNoise::OctaveNoise(int octave, float x, float y, float z)
{
	float result = 0.0f;
	float amp = 1.0f;

	for (int i = 0; i < octave; i++)
	{
		result += Noise(x, y, z) * amp;
		x *= 2.0f;
		y *= 2.0f;
		z *= 2.0f;
		amp *= 0.5f;
	}

	return result;
}

//-------------------------------------GRAD------------------------------------
// Gets gradiant direction from lo 4 bits of hash code.
//-----------------------------------------------------------------------------
float PerlinNoise::Grad(unsigned int hash, float x, float y, float z)
{
	const int h = hash & 15;
	const float u = (h < 8) ? x : y;
	const float v = (h < 4) ? y : (h == 12 || h == 14) ? x : z;
	return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

//-------------------------------GENERATE_PERMUTATIONS-------------------------
//  The foundation of improved perlin is to use a permutation table which acts
//  like a hash code for the Gradiant.
//-----------------------------------------------------------------------------
void PerlinNoise::GeneratePermutations(unsigned int seed)
{
	//Fill the perm table with 0 - 256.
	for (unsigned int i = 0; i < 256; i++)
	{
		p[i] = i;
	}

	//Shufle the array using seed
	std::shuffle(std::begin(p), std::begin(p) + 256, std::default_random_engine(seed));

	//Now duplicate the array
	for (size_t i = 0; i < 256; ++i)
	{
		p[256 + i] = p[i];
	}
}


