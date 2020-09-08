#include "Math/Random.h"
#include "Math/Mathf.h"
#include <chrono>

Random::Random()
{
	mx = (unsigned int)std::chrono::system_clock::now().time_since_epoch().count();
	my = Shift32(mx);
	mz = Shift32(my);
	mw = Shift32(mz);
	mc = 0;
}

Random::Random(unsigned int seed)
{
	if (seed == 0) { seed = (unsigned int)std::chrono::system_clock::now().time_since_epoch().count(); }
	mx = seed;
	my = Shift32(mx);
	mz = Shift32(my);
	mw = Shift32(mz);
	mc = 0;
}

//------------------------------------DESTRUCTOR-------------------------------
Random::~Random()
{
}

float Random::Next()
{
	//Type punning, undefined behavour but works.
	union
	{
		unsigned int mI;
		float mF;
	} toFlot;
	//0x3f800000 == E == 1.0, now we pick random mantsia to make value random
	//mantisa capped between 1 and 2?
	toFlot.mI = 0x3f800000 | (xorwow() >> 9);
	return toFlot.mF - 1.0f; //Return the float value between 0 and 1
}

int Random::Range(int min, int max)
{
	return min + (int)((max - min) * Next());
}

float Random::Range(float min, float max)
{
	return min + ((max - min) * Next());
}

Vector2 Random::PointInCircle(float radius)
{
	//Polar coordinates
	float a = Mathf::PI_2 * Next(); //Circumferance 2PI
	float r = Next() * Mathf::Sqrt(radius); //Intergral of 2X is X^2 y = sqrt(x)

	//Return as Cartisean coordinates.
	return Vector2(r * Mathf::Cos(a), r * Mathf::Sin(a));
}

Vector2 Random::PointOnCircle(float radius)
{
	//Polar coordinates
	float angle = Next() * Mathf::PI_2; //Circumferance 2PI

	//Return as Cartisean coordinates.
	return Vector2(radius * Mathf::Cos(angle), radius * Mathf::Sin(angle));
}

Vector3 Random::PointInSphere(float radius)
{
	float theta = Mathf::PI_2 * Next();
	float phi = Mathf::Acos(1 - 2.0f * Next());
	float r = radius * Next();

	float rSinPhi = r * Mathf::Sin(phi);

	return Vector3(rSinPhi * Mathf::Cos(theta), rSinPhi * Mathf::Sin(theta), r * Mathf::Cos(phi));
}

Vector3 Random::PointOnSphere(float radius)
{
	float theta = Mathf::PI_2 * Next();
	float phi = Mathf::Acos(1 - 2.0f * Next());

	float rSinPhi = radius * Mathf::Sin(phi);

	return Vector3(rSinPhi * Mathf::Cos(theta), rSinPhi * Mathf::Sin(theta), radius * Mathf::Cos(phi));
}

//------------------------------------SHIFT32---------------------------------
// This is the barebones Xor, it makes a random 32bit value by XOR shifting
// in a recursion manner, think fibbonnaci sequence.
//----------------------------------------------------------------------------
unsigned int Random::Shift32(unsigned int x)
{
	x ^= x << 13;
	x ^= x >> 17;
	x ^= x << 5;
	return x;
}

unsigned int Random::xorwow()
{
	unsigned int t = mw;

	// Shuffle the states around.
	unsigned int const s = mx;
	mw = mz;
	mz = my;
	my = s;

	// XOR the value
	t ^= t >> 2;
	t ^= t << 1;
	t ^= s ^ (s << 4);
	mx = t;

	// Increment the counter
	mc += 362437;
	return t + mc;
}
