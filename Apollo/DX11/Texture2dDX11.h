#pragma once

#include "TextureResource.h"

namespace Apollo
{
	class Texture2dDX11 : public TextureResource
	{
	public:

		Texture2dDX11(const std::string& path, uint32_t handle, ID3D11ShaderResourceView* srv);
		virtual ~Texture2dDX11();

		ID3D11ShaderResourceView*		getSRV() { return m_textureSRVPtr.Get(); }

	protected:

		ShaderResourceViewComPtr		m_textureSRVPtr;
		DepthStencilViewComPtr			m_depthStencilViewPtr;;
		RenderTargetViewComPtr			m_renderTargetViewPtr;;
		UnorderedAccessViewComPtr	m_unorderedAccessViewPtr;
	};
}
