#include "stdafx.h"
#include "Texture3dConfigDX11.h"

NAME_SPACE_BEGIN_APOLLO

Texture3dConfigDX11::Texture3dConfigDX11()
{
	SetDefaults();
}
//--------------------------------------------------------------------------------
Texture3dConfigDX11::~Texture3dConfigDX11()
{
}
//--------------------------------------------------------------------------------
void Texture3dConfigDX11::SetDefaults()
{
	// Set the state to the default configuration.  These are the D3D11 default
	// values as well.

	m_State.Width = 1;
	m_State.Height = 1;
	m_State.Depth = 1;
	m_State.MipLevels = 1;
	m_State.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	m_State.Usage = D3D11_USAGE_DEFAULT;
	m_State.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	m_State.CPUAccessFlags = 0;
	m_State.MiscFlags = 0;
}
//--------------------------------------------------------------------------------
void Texture3dConfigDX11::SetWidth(UINT state)
{
	m_State.Width = state;
}
//--------------------------------------------------------------------------------
void Texture3dConfigDX11::SetHeight(UINT state)
{
	m_State.Height = state;
}
//--------------------------------------------------------------------------------
void Texture3dConfigDX11::SetDepth(UINT state)
{
	m_State.Depth = state;
}
//--------------------------------------------------------------------------------
void Texture3dConfigDX11::SetMipLevels(UINT state)
{
	m_State.MipLevels = state;
}
//--------------------------------------------------------------------------------
void Texture3dConfigDX11::SetFormat(DXGI_FORMAT state)
{
	m_State.Format = state;
}
//--------------------------------------------------------------------------------
void Texture3dConfigDX11::SetUsage(D3D11_USAGE state)
{
	m_State.Usage = state;
}
//--------------------------------------------------------------------------------
void Texture3dConfigDX11::SetBindFlags(UINT state)
{
	m_State.BindFlags = state;
}
//--------------------------------------------------------------------------------
void Texture3dConfigDX11::SetCPUAccessFlags(UINT state)
{
	m_State.CPUAccessFlags = state;
}
//--------------------------------------------------------------------------------
void Texture3dConfigDX11::SetMiscFlags(UINT state)
{
	m_State.MiscFlags = state;
}
//--------------------------------------------------------------------------------
D3D11_TEXTURE3D_DESC Texture3dConfigDX11::GetTextureDesc()
{
	return(m_State);
}

NAME_SPACE_END