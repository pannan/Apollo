#pragma once
#include "SampleBase.h"
#include "ModelDX11.h"
#include "ConstantBufferDX11.h"
#include "RenderStateDX11.h"
#include "Scene.h"
#include "RenderPass.h"
#include "RenderPipeline.h"
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

		void			initDeferredPipeline();

		void			initQuadMesh();

	protected:

		Camera*					m_camera;

		ModelDX11				m_modelScene;

		ConstantBufferDX11Ptr m_matrixBuffer;

		RenderStateDX11 m_renderState;

		RenderStateDX11 m_deferredGBufferRenderState;

		ScenePtr				m_scenePtr;

		RenderPassPtr		m_renderPassPtr;

		RenderPassPtr		m_deferredGBufferPassPtr;
		RenderPassPtr		m_deferredLightingPassPtr;
	//	std::vector<IRenderable*>		m_renderableList;

		RenderPipelinePtr		m_forwardPipelinePtr;

		RenderPipelinePtr		m_deferredPiplinePtr;

		ModelDX11					m_deferredLightingQuadModel;

	private:
	};	  
}
