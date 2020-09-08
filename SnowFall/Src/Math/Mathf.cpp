#include "Math/Mathf.h"

namespace Mathf
{
	float ToRadians(float deg)
	{
		return deg * DEG_TO_RAD;
	}

	float ToDegrees(float rad)
	{
		return rad * RAD_TO_DEG;
	}

	float Clamp(float value, float min, float max)
	{
		if (value >= max)
		{
			return max;
		}

		if (value <= min)
		{
			return min;
		};

		return value;
	}

	float Clamp01(float value)
	{
		if (value >= 1)
		{
			return 1.0f;
		}

		if (value <= 0)
		{
			return 0.0f;
		};

		return value;
	}

	int Clamp(int value, int min, int max)
	{
		if (value >= max)
		{
			return max;
		}

		if (value <= min)
		{
			return min;
		};

		return value;
	}

	int Clamp01(int value)
	{
		if (value >= 1)
		{
			return 1;
		}

		if (value <= 0)
		{
			return 0;
		};

		return value;
	}

	unsigned int Clamp(unsigned int value, unsigned int min, unsigned int max)
	{
		if (value >= max)
		{
			return max;
		}

		if (value <= min)
		{
			return min;
		};

		return value;
	}

	unsigned int Clamp01(unsigned int value)
	{
		if (value >= 1)
		{
			return 1;
		}

		if (value <= 0)
		{
			return 1;
		};

		return value;
	}

	float Max(float a, float b)
	{
		return (a > b) ? a : b;
	}

	int Max(int a, int b)
	{
		return (a > b) ? a : b;
	}

	unsigned int Max(unsigned int a, unsigned int b)
	{
		return (a > b) ? a : b;
	}

	float Min(float a, float b)
	{
		return (a < b) ? a : b;
	}

	int Min(int a, int b)
	{
		return (a < b) ? a : b;
	}

	float Sin(float a)
	{
		return sinf(a);
	}

	float Cos(float a)
	{
		return cosf(a);
	}

	float Tan(float a)
	{
		return tanf(a);
	}

	float Asin(float x)
	{
		return asinf(x);
	}

	float Acos(float x)
	{
		return acosf(x);
	}

	float Atan(float x)
	{
		return atanf(x);
	}

	float Atan2(float y, float x)
	{
		return atan2f(y, x);
	}

	unsigned int Abs(int x)
	{
		return abs(x);
	}

	float Abs(float x)
	{
		return fabsf(x);
	}

	float Sqrt(float x)
	{
		return sqrtf(x);
	}

	float RecipSqrt(float x)
	{
		return 1.0f / sqrtf(x);
	}

	float Log2(float x)
	{
		return log2f(x);
	}

	float Ceil(float x)
	{
		return ceilf(x);
	}

	float Floor(float x)
	{
		return floorf(x);
	}

	float Pow(float value, float exponent)
	{
		return powf(value, exponent);
	}

	bool IsInfinite(float x)
	{
		return isinf(x);
	}

	bool IsNan(float x)
	{
		return isnan(x);
	}

	bool IsZero(float x)
	{
		return (Abs(x) <= EPSILLON);
	}

	bool IsEqual(float a, float b)
	{
		return (Abs(a - b) <= EPSILLON * (Abs(a) + Abs(b) + 1.0f));
	}

	bool IsEven(int x)
	{
		return (x % 2) == 0;
	}

	bool IsOdd(int x)
	{
		return (x % 2) != 0;
	}

	float Lerp(float v1, float v2, float t)
	{
		return (1 - t) * v1 + (v2 * t);
	}

	float SmoothStep(float min, float max, float t)
	{
		t = Clamp01((t - min) / (min - max));
		return t * t * (3 - 2 * t);
	}

	float SmoothStepKP(float min, float max, float t)
	{
		t = Clamp01((t - min) / (min - max));
		return t * t * t * (t * (t * 6 - 15) + 10);
	}

	float CatmullRom(float A, float B, float C, float D, float t)
	{
		float tSqr = t * t;
		float tCubed = tSqr * t;
		return (float)(0.5f * (2.0f * B + (C - A) * t +
			(2.0f * A - 5.0f * B + 4.0f * C - D) * tSqr +
			(3.0f * B - A - 3.0f * C + D) * tCubed));
	}

	float Bezier(float A, float B, float C, float t)
	{
		t = Clamp01(t);
		float	oneMinusT = 1.0f - t;
		return	oneMinusT * oneMinusT * A +
			2.0f * oneMinusT * t * B +
			t * t * C;
	}

	float BezierCubic(float A, float B, float C, float D, float t)
	{
		t = Clamp01(t);
		float	oneMinusT = 1.0f - t;
		return	oneMinusT * oneMinusT * oneMinusT * A +
			3.0f * oneMinusT * oneMinusT * t * B +
			3.0f * oneMinusT * t * t * C +
			t * t * t * D;
	}

	//http://graphics.stanford.edu/~seander/bithacks.html
	Uint32 NextPowerOfTwo(Uint32 v)
	{
		//Seems too only retain highest order bit or something
		//i cannot find a good explanation for this.
		v--;
		v |= v >> 1;
		v |= v >> 2;
		v |= v >> 4;
		v |= v >> 8;
		v |= v >> 16;
		return v + 1;
	}

	Uint32 PreviousPowerOfTwo(Uint32 v)
	{
		return NextPowerOfTwo(v + 1) / 2;
	}

	Uint32 NearestPowerOfTwo(Uint32 v)
	{
		const Uint32 np2 = NextPowerOfTwo(v);
		const Uint32 pp2 = PreviousPowerOfTwo(v);

		// Find the smallest delta and return accordingly.
		return ((np2 - v) <= (v - pp2)) ? np2 : pp2;
	}

	float ToSrgbFast(float f)
	{
		// Close Approximation to SRGB
		f = Clamp01(f);
		float s1 = Sqrt(f);
		float s2 = Sqrt(s1);
		float s3 = Sqrt(s2);
		return 0.662002687f * s1 + 0.684122060f * s2 - 0.323583601f * s3 - 0.0225411470f * f;
	}

	float ToLinear(float f)
	{
		f = Clamp01(f);
		return f * (f * (f * 0.305306011f + 0.682171111f) + 0.012522878f);
	}

};