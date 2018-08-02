#pragma once

#include "Vector4.h"

namespace Apollo
{
	class Texture2dDX11;
	class StructuredBufferDX11;

	enum class AttachmentPoint : uint8_t
	{
		Color0,         // Must be a uncompressed color format.
		Color1,         // Must be a uncompressed color format.
		Color2,         // Must be a uncompressed color format.
		Color3,         // Must be a uncompressed color format.
		Color4,         // Must be a uncompressed color format.
		Color5,         // Must be a uncompressed color format.
		Color6,         // Must be a uncompressed color format.
		Color7,         // Must be a uncompressed color format.
		Depth,          // Must be a texture with a depth format.
		DepthStencil,   // Must be a texture with a depth/stencil format.
		NumAttachmentPoints
	};

	class RenderTargetDX11
	{
	public:

		RenderTargetDX11();
		~RenderTargetDX11();

		void										attachTexture(AttachmentPoint attachment, Texture2dDX11* tex);

		Texture2dDX11*					getTexture(AttachmentPoint attachment);

		void										clear(AttachmentPoint attachment, ClearFlags clearFlags = ClearFlags::All,const Vector4& color = Vector4::ZERO,
															float depth = 1.0f,uint8_t stencil = 0);

		void										clear(ClearFlags clearFlags = ClearFlags::All, const Vector4& color = Vector4::ZERO,float depth = 1.0f, uint8_t stencil = 0);

		void										attachStructuredBuffer(uint8_t slot,StructuredBufferDX11* structuredBuffer);

		StructuredBufferDX11*			getStructuredBuffer(uint8_t slot);

		void										resize(int width, int height);

		void										bind();

		void										unBind();	

	protected:

		int										m_width;
		int										m_height;

		ID3D11Device*					m_device;
		ID3D11DeviceContext*		m_deviceContext;

		std::vector<Texture2dDX11*>			m_textureList;

		std::vector<StructuredBufferDX11*>	m_structuredBufferList;

	private:
	};
}
