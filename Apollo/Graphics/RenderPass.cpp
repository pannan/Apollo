#include "stdafx.h"
#include "RenderPass.h"


using namespace Apollo;

void RenderPass::preRender(RenderState& renderState)
{

}

void RenderPass::render(RenderState& renderState)
{
	if (m_scene)
		m_scene->render();
}

void RenderPass::postRender(RenderState& renderState)
{

}