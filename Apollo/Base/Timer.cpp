#include "stdafx.h"
#include "Timer.h"

using namespace Apollo;

Timer::Timer()
{
	m_iFramesPerSecond = 0;
	m_iMaxFramesPerSecond = 0;
	m_iFrameCount = 0;

	m_fDelta = 0;
	m_fFixedDelta = 0.0f;
	m_bUseFixedStep = false;

	QueryPerformanceFrequency((LARGE_INTEGER*)&m_TicksPerSecond64);
	QueryPerformanceCounter((LARGE_INTEGER*)&m_CurrentTicks64);
	m_StartupTicks64 = m_CurrentTicks64;
	m_OneSecTicks64 = m_CurrentTicks64;
}
//--------------------------------------------------------------------------------
Timer::~Timer()
{
}
//--------------------------------------------------------------------------------
void Timer::reset()
{
	m_iFramesPerSecond = 0;
	m_iFrameCount = 0;
	m_fDelta = 0;
}
//--------------------------------------------------------------------------------
void Timer::setFixedTimeStep(float step)
{
	if (step <= 0.0f)
	{
		m_bUseFixedStep = false;
		m_fFixedDelta = 0.0f;
	}
	else
	{
		m_bUseFixedStep = true;
		m_fFixedDelta = step;
	}
}
//--------------------------------------------------------------------------------
void Timer::update()
{
	m_LastTicks64 = m_CurrentTicks64;
	QueryPerformanceCounter((LARGE_INTEGER*)&m_CurrentTicks64);

	// Update the time increment

	if (m_bUseFixedStep)
		m_fDelta = m_fFixedDelta;
	else
		m_fDelta = (float)((__int64)m_CurrentTicks64 - (__int64)m_LastTicks64) / (__int64)m_TicksPerSecond64;

	// Continue counting the frame rate regardless of the time step.

	if ((float)((__int64)m_CurrentTicks64 - (__int64)m_OneSecTicks64)
		/ (__int64)m_TicksPerSecond64 < 1.0f)
	{
		m_iFrameCount++;
	}
	else
	{
		m_iFramesPerSecond = m_iFrameCount;

		if (m_iFramesPerSecond > m_iMaxFramesPerSecond)
			m_iMaxFramesPerSecond = m_iFramesPerSecond;

		m_iFrameCount = 0;
		m_OneSecTicks64 = m_CurrentTicks64;
	}

}
//--------------------------------------------------------------------------------
float Timer::elapsed()
{
	return(m_fDelta);
}
//--------------------------------------------------------------------------------
int Timer::framerate()
{
	return(m_iFramesPerSecond);
}
//--------------------------------------------------------------------------------
float Timer::runtime()
{
	return ((float)((__int64)m_CurrentTicks64 - (__int64)m_StartupTicks64)
		/ (__int64)m_TicksPerSecond64);
}
//--------------------------------------------------------------------------------
int Timer::maxFramerate()
{
	return(m_iMaxFramesPerSecond);
}
//--------------------------------------------------------------------------------
int Timer::frameCount()
{
	return(m_iFrameCount);
}
//--------------------------------------------------------------------------------
float Timer::frametime()
{
	return(1.0f / static_cast<float>(m_iFramesPerSecond));
}