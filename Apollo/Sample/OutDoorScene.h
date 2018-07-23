#pragma once
#include "SampleBase.h"
namespace Apollo
{
	class OutDoorScene : public SampleBase
	{
		OutDoorScene();
		~OutDoorScene();

		virtual void  init();

		virtual void	render();

		virtual Camera* getCamera() { return m_camera; }

	protected:

		Camera*					m_camera;

	private:
	};	  
}
