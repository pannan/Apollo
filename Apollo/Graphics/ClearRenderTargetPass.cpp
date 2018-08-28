#include "stdafx.h"
#include "ClearRenderTargetPass.h"
#include "Texture2dDX11.h"

using namespace Apollo;

ClearRenderTargetPass::ClearRenderTargetPass(Texture2dDX11* texture,
	ClearFlags clearFlags /* = ClearFlags::All */,
	const Vector4& color /* = Vector4::ZERO */,
	float depth /* = 1.0f */,
	uint8_t stencil /* = 0 */) :
	m_texture(texture), m_clearColor(color), m_clearFlags(clearFlags), m_clearDepth(depth), m_clearStencil(stencil)
{

}

ClearRenderTargetPass::ClearRenderTargetPass(RenderTargetDX11Ptr& renderTarget,
	ClearFlags clearFlags /* = ClearFlags::All */,
	const Vector4& color /* = Vector4::ZERO */,
	float depth /* = 1.0f */,
	uint8_t stencil /* = 0 */) :
	m_renderTarget(renderTarget), m_clearColor(color), m_clearFlags(clearFlags), m_clearDepth(depth), m_clearStencil(stencil),m_texture(nullptr)
{

}

ClearRenderTargetPass::~ClearRenderTargetPass()
{

}

void ClearRenderTargetPass::render()
{
	if (m_texture)
		m_texture->clear(m_clearFlags, m_clearColor, m_clearDepth, m_clearStencil);

	if (m_renderTarget)
		m_renderTarget->clear(m_clearFlags, m_clearColor, m_clearDepth, m_clearStencil);
}
