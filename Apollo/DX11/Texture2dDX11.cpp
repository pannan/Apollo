#include "stdafx.h"
#include "Texture2dDX11.h"

using namespace Apollo;

Texture2dDX11::Texture2dDX11(const std::string& path, uint32_t handle) : TextureResource(path, handle, TextureType_2D)
{

}

Texture2dDX11::~Texture2dDX11()
{

}