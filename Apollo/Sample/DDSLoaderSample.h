#pragma once

#include "SampleBase.h"

namespace Apollo
{
	class Camera;

	class DDSLoaderSample : public SampleBase
	{
	public:
		DDSLoaderSample();
		~DDSLoaderSample();

		virtual void  init();

		virtual void	render();

		virtual void onGUI();

		virtual Camera* getCamera() { return m_camera; }

	protected:

	

	private:

		Camera * m_camera;
		

	};
}
