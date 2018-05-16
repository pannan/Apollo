#pragma once

#include "TextureResource.h"

namespace Apollo
{
	class Texture2dDX11 : public TextureResource
	{
	public:
		Texture2dDX11(const std::string& path, uint32_t handle, ID3D11ShaderResourceView* srv);
		virtual ~Texture2dDX11();

	protected:

		ShaderResourceViewComPtr		m_textureSRVPtr;
	};
}
