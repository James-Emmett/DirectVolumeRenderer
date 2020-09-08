#pragma once
#include "System/Types.h"
#include <math.h>

namespace Mathf
{
	const float EPSILLON    = 1.0e-6f;
	const float E           = 2.7182818284590452353602874713527f;
	const float Log10E      = 0.4342944819032518f;
	const float Log2E       = 1.4426950408889634f;
	const float PI          = 3.1415926535897932384626433832795f;
	const float PI_OVER_2   = 1.5707963267948966192313216916398f;
	const float PI_OVER_4   = 0.785398163397448309616f;
	const float PI_2        = 2.0f * PI;
	const float DEG_TO_RAD  = 0.0174532924f;
	const float RAD_TO_DEG  = 57.29578f;
	const float FLOAT_MIN   = 1E-37f;
	const float FLOAT_MAX   = 1E+37f;

	extern inline float ToRadians(float deg);
	extern inline float ToDegrees(float rad);
	extern inline float Clamp(float value, float min, float max);
	extern inline float Clamp01(float value);
	extern inline int Clamp(int value, int min, int max);
	extern inline int Clamp01(int value);
	extern inline unsigned int Clamp(unsigned int value, unsigned int min, unsigned int max);
	extern inline unsigned int Clamp01(unsigned int value);
	extern inline float Max(float a, float b);
	extern inline int Max(int a, int b);
	extern inline unsigned int Max(unsigned int a, unsigned int b);
	extern inline float Min(float a, float b);
	extern inline int Min(int a, int b);
    extern inline float Sin(float a);
	extern inline float Cos(float a);
	extern inline float Tan(float a);
	extern inline float Asin(float x);
	extern inline float Acos(float x);
	extern inline float Atan(float x);
	extern inline float Atan2(float y, float x);
	extern inline unsigned int Abs(int x);
	extern inline float Abs(float x);
	extern inline float Sqrt(float x);
	extern inline float RecipSqrt(float x);
	extern inline float Log2(float x);
	extern inline float Ceil(float x);
	extern inline float Floor(float x);
	extern inline float Pow(float value, float exponent);
	extern inline bool IsInfinite(float x);
	extern inline bool IsNan(float x);
	extern inline bool IsZero(float x);
	extern inline bool IsEqual(float a, float b);
	extern inline bool IsEven(int x);
	extern inline bool IsOdd(int x);
	extern inline float Lerp(float v1, float v2, float t);
	extern inline float SmoothStep(float min, float max, float t);
	extern inline float SmoothStepKP(float min, float max, float t);
	extern inline float CatmullRom(float A, float B, float C, float D, float t);
	extern inline float Bezier(float A, float B, float C, float t);
	extern inline float BezierCubic(float A, float B, float C, float D, float t);

	//http://graphics.stanford.edu/~seander/bithacks.html
	extern inline Uint32 NextPowerOfTwo(Uint32 v);
	extern inline Uint32 PreviousPowerOfTwo(Uint32 v);
	extern inline Uint32 NearestPowerOfTwo(Uint32 v);
	extern inline float ToSrgbFast(float f);
	extern inline float ToLinear(float f);

}