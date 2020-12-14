#pragma once

#include "SampleBase.h"

namespace Apollo
{
	class Camera;

	class TGALoaderSample : public SampleBase
	{
	public:
		TGALoaderSample();
		~TGALoaderSample();

		virtual void  init();

		virtual void	render();

		virtual void onGUI();

		virtual Camera* getCamera() { return m_camera; }

	protected:

	

	private:

		Camera * m_camera;
		

	};
}
