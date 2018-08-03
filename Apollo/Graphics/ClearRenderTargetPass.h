#pragma once

#include "RenderPass.h"
#include "RenderTargetDX11.h"

namespace Apollo
{
	class ClearRenderTargetPass : public RenderPass
	{
	public:
		ClearRenderTargetPass(	RenderTargetDX11Ptr& renderTarget,
												ClearFlags clearFlags = ClearFlags::All,
												const Vector4& color = Vector4::ZERO,
												float depth = 1.0f,
												uint8_t stencil = 0);

		ClearRenderTargetPass(	Texture2dDX11* texture,
												ClearFlags clearFlags = ClearFlags::All,
												const Vector4& color = Vector4::ZERO,
												float depth = 1.0f,
												uint8_t stencil = 0);

		virtual ~ClearRenderTargetPass();

		virtual void render();

	private:
		RenderTargetDX11Ptr m_renderTarget;
		Texture2dDX11*			m_texture;
		ClearFlags					m_clearFlags;
		Vector4						m_clearColor;
		float								m_clearDepth;
		uint8_t							m_clearStencil;
	};
}
