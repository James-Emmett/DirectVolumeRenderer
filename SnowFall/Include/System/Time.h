#pragma once
#include "Types.h"
#include <string>
#include <cstdint>

class Time
{
private:
	float m_DeltaTime = 0;
	int   m_FPS = 0;
	int   m_MaxFPS = 0;
	int   m_MinFPS = INT32_MAX;
	int   m_Frame = 0;      // Total frame count
	int   m_FrameCount = 0; // Track Frame count during 1 second period to create new FPS
	float m_FrameCap = 0;   // Good for laptops

	//--statics managign fixed time--
	static float gFixedTimeStep;
	static float gTimeScale;

	uint_least64_t m_StartTime = 0; //Time game started
	uint_least64_t m_CurrentTime = 0; //Current time this frame
	uint_least64_t m_PreviousTime = 0; //Time of previous frame
	uint_least64_t m_OneSecondTimer = 0; //Times 1 second for fps counter

public:
	Time();
	~Time() {};

public:
	void  Start();
	void  Update();
	void  SetFrameCap(int cap);
	float GetFrameCapStep();
	float TimeSinceStart()const;
	float FrameTime()const;
	float DeltaTime()const;
	int	  FPS()const;
	int	  MaxFPS()const;
	int	  MinFPS()const;
	int	  Frame()const;

	//--Statics--
	static Uint64 CurrentTimeMilliseconds();
	static Uint64 CurrentTimeSeconds();
	static std::string CurrentTimeAndDate();
	static float FixedTimeStep();
	static float TimeScale();
	static void SetTimeScale(float value);
	static void SetFixedTimeStep(float value);
};