#pragma once

#include "Singleton.h"

namespace Apollo
{
	class Timer : public SingletonEx<Timer>
	{
	public:
		Timer();
		~Timer();

		void update();

		void reset();

		float runtime();

		float elapsed();

		int framerate();

		int maxFramerate();

		int frameCount();

		float frametime();

		void setFixedTimeStep(float step);

	private:

		float m_fDelta;
		int m_iFramesPerSecond;
		int m_iMaxFramesPerSecond;
		int m_iFrameCount;

		float m_fFixedDelta;
		bool m_bUseFixedStep;

		unsigned __int64 m_TicksPerSecond64;
		unsigned __int64 m_StartupTicks64;
		unsigned __int64 m_CurrentTicks64;
		unsigned __int64 m_OneSecTicks64;
		unsigned __int64 m_LastTicks64;
	};
}
