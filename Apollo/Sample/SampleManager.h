#pragma once

#include "Singleton.h"
#include "QueryDX11.h"
namespace Apollo
{
	class SampleBase;

	class SampleManager : public Singleton<SampleManager>
	{
	public:

		SampleManager();
		~SampleManager();

		void	init();

		void		preRender();

		void		render();

		void		postRender();

		void debugOverlay();

	protected:

		

	protected:

		SampleBase*		m_currentSample;

		std::vector<SampleBase*>	m_sampleList;

		QueryDX11*			m_query;

		float				m_gpuFrameTimes;

	private:
	};
}
