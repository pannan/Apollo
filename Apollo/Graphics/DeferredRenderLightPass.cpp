#include "stdafx.h"
#include "DeferredRenderLightPass.h"
#include "RenderState.h"

using namespace Apollo;

DeferredRenderLightPass::DeferredRenderLightPass() : RenderPass()
{

}

DeferredRenderLightPass::DeferredRenderLightPass(ScenePtr& scene, RenderState* renderState) : RenderPass(scene, renderState)
{

}

void DeferredRenderLightPass::preRender(RenderState* renderState)
{
	if (m_renderState)
		m_renderState->bind();
}

void DeferredRenderLightPass::render(RenderState* renderState)
{
	if (m_scene)
		m_scene->render();
}

void DeferredRenderLightPass::postRender(RenderState* renderState)
{
	if (m_renderState)
		m_renderState->unBind();
}