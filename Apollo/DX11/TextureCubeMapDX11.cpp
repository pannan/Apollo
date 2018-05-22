#include "stdafx.h"
#include "TextureCubeMapDX11.h"

using namespace Apollo;

TextureCubeMapDX11::TextureCubeMapDX11(const std::string& path,
	uint32_t handle,
	ID3D11ShaderResourceView* srv) :
	TextureResource(path, handle, TextureType_CubeTex),
	m_textureSRVPtr(srv)
{

}

TextureCubeMapDX11::~TextureCubeMapDX11()
{

}