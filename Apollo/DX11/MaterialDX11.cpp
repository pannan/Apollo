#include "stdafx.h"
#include "MaterialDX11.h"

using namespace Apollo;

RenderPipelineType MaterialDX11::s_currentRenderPipeline = RenderPipelineType::ForwardRender;

void MaterialDX11::bind() const
{
	if (m_vs)
		m_vs->bin();

	if (m_ps[(uint8_t)s_currentRenderPipeline])
		m_ps[(uint8_t)s_currentRenderPipeline]->bin();
}

void MaterialDX11::unBind() const
{
	if (m_vs)
		m_vs->unBind();

	if (m_ps[(uint8_t)s_currentRenderPipeline])
		m_ps[(uint8_t)s_currentRenderPipeline]->unBind();
}