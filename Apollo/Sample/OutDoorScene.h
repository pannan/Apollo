#pragma once
#include "SampleBase.h"
#include "ModelDX11.h"
#include "ConstantBufferDX11.h"
#include "RenderStateDX11.h"
namespace Apollo
{
	//class IRenderable;
	class OutDoorScene : public SampleBase
	{
	public:
		OutDoorScene();
		~OutDoorScene();

		virtual void  init();

		virtual void	render();

		virtual Camera* getCamera() { return m_camera; }

	protected:

		Camera*					m_camera;

		ModelDX11				m_modelScene;

		ConstantBufferDX11Ptr m_matrixBuffer;

		RenderStateDX11 m_renderState;

	//	std::vector<IRenderable*>		m_renderableList;

	private:
	};	  
}
