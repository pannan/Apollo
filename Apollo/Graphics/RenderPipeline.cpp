#include "stdafx.h"
#include "RenderPipeline.h"

using namespace Apollo;

void RenderPipeline::render(RenderState* renderState)
{
	for (auto pass : m_renderPassList)
	{
		if (pass->getEnable())
		{
			pass->preRender(renderState);
			pass->render(renderState);
			pass->postRender(renderState);
		}
	}
}