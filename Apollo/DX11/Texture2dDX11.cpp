#include "stdafx.h"
#include "Texture2dDX11.h"

using namespace Apollo;

Texture2dDX11::Texture2dDX11(const std::string& path, 
													uint32_t handle, 
													ID3D11ShaderResourceView* srv) :
													TextureResource(path, handle, TextureType_2D),
													m_textureSRVPtr(srv)
{

}

Texture2dDX11::~Texture2dDX11()
{

}