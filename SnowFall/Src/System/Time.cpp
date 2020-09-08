#include "System/Time.h"
#include <chrono>
#include <ctime>

float Time::gFixedTimeStep = 0.02f;
float Time::gTimeScale = 1.0f;

Time::Time()
{
	m_DeltaTime = 0;
	m_FPS = 0;
	m_MaxFPS = 0;
	m_FrameCount = 0;
	m_FrameCap = 60;
}

void Time::Start()
{
	m_DeltaTime = 0;
	m_FPS = 0;
	m_MaxFPS = 0;
	m_FrameCount = 0;
	m_FrameCap = 60;

	//Get the current system time in microseconds
	m_CurrentTime = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()).time_since_epoch().count();

	m_StartTime = m_CurrentTime;
	m_OneSecondTimer = m_CurrentTime;
	m_PreviousTime = 0;
}

void Time::Update()
{
	m_Frame++;
	m_PreviousTime = m_CurrentTime;
	m_CurrentTime = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()).time_since_epoch().count();

	// Time in seconds since last frame
	m_DeltaTime = ((float)(m_CurrentTime - m_PreviousTime) * 0.001f) * gTimeScale;

	// Check if 1 second has passed
	if ((m_CurrentTime - m_OneSecondTimer) * 0.001f < 1.0f)
	{
		m_FrameCount++;
	}
	else
	{
		m_FPS = m_FrameCount;

		if (m_FPS > m_MaxFPS) { m_MaxFPS = m_FPS; }
		if (m_FPS < m_MinFPS) { m_MinFPS = m_FPS; }

		m_FrameCount = 0;
		m_OneSecondTimer = m_CurrentTime;
	}
}

void Time::SetFrameCap(int cap)
{
	m_FrameCap = (float)cap;
}

float Time::GetFrameCapStep()
{
	return 1.0f / m_FrameCap;
}

float Time::TimeSinceStart()const
{
	return (float)((m_CurrentTime - m_StartTime) * 0.001f);
}

float Time::FrameTime()const
{
	return 1.0f / m_FPS;
}

float Time::DeltaTime()const
{
	return m_DeltaTime;
}

int Time::FPS()const
{
	return m_FPS;
}

int Time::MaxFPS()const
{
	return m_MaxFPS;
}

int Time::MinFPS()const
{
	return m_MinFPS;
}

int Time::Frame() const
{
	return m_Frame;
}

Uint64 Time::CurrentTimeMilliseconds()
{
	return (Uint64)std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()).time_since_epoch().count();
}

Uint64 Time::CurrentTimeSeconds()
{
	return (Uint64)std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::steady_clock::now()).time_since_epoch().count();
}

std::string Time::CurrentTimeAndDate()
{
	std::time_t today = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	tm localTime;
	localtime_s(&localTime, &today);

	char s[128];
	strftime(s, 128, "%c", &localTime);
	return std::string(s);
}

float Time::FixedTimeStep()
{
	return gFixedTimeStep;
}

float Time::TimeScale()
{
	return gTimeScale;
}

void Time::SetTimeScale(float value)
{
	gTimeScale = value;
}

void Time::SetFixedTimeStep(float value)
{
	gFixedTimeStep = value;
}