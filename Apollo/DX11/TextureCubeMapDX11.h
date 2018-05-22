#pragma once

#include "TextureResource.h"

namespace Apollo
{
	class TextureCubeMapDX11 : public TextureResource
	{
	public:

		TextureCubeMapDX11(const std::string& path, uint32_t handle, ID3D11ShaderResourceView* srv);
		virtual ~TextureCubeMapDX11();

		ID3D11ShaderResourceView*		getSRV() { return m_textureSRVPtr.Get(); }

	protected:

		ShaderResourceViewComPtr		m_textureSRVPtr;
	};
}
