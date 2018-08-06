#include "stdafx.h"
#include "RenderPass.h"
#include "RenderState.h"

using namespace Apollo;

RenderPass::RenderPass(ScenePtr& scene, RenderState* renderState) : m_scene(scene), m_renderState(renderState)
{

}

void RenderPass::preRender(RenderState& renderState)
{
	if (m_renderState)
		m_renderState->bind();
}

void RenderPass::render(RenderState& renderState)
{
	if (m_scene)
		m_scene->render();
}

void RenderPass::postRender(RenderState& renderState)
{
	if (m_renderState)
		m_renderState->unBind();
}