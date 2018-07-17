#pragma once

#include "Singleton.h"

namespace Apollo
{
	class SampleBase;

	class SampleManager : public Singleton<SampleManager>
	{
	public:

		SampleManager();
		~SampleManager();

		void	init();

		void render();

		void debugOverlay();

	protected:

		

	protected:

		SampleBase*		m_currentSample;

		std::vector<SampleBase*>	m_sampleList;

	private:
	};
}
