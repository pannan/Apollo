#include "stdafx.h"
#include "Texture2dDX11.h"
#include "RendererDX11.h"

using namespace Apollo;

Texture2dDX11::Texture2dDX11(const std::string& path,
	uint32_t handle) :
	TextureResource(path, handle, TextureType_2D)
{

}

Texture2dDX11::Texture2dDX11(	const std::string& path, 
													uint32_t handle, 
													D3D11_TEXTURE2D_DESC desc,
													ID3D11Texture2D* texture2d,
													DepthStencilViewComPtr dsv,
													RenderTargetViewComPtr rtv):
													TextureResource(path, handle, TextureType_2D),		
													m_tex2dDesc(desc),
													m_tex2dDx11(texture2d),
													m_depthStencilViewPtr(dsv),
													m_renderTargetViewPtr(rtv)
{

}

Texture2dDX11::~Texture2dDX11()
{
	int ii = 0;
}

void Texture2dDX11::bind(UINT slotID, ShaderType shaderType, ShaderParameterType parameterType)
{
}

void Texture2dDX11::unBind(UINT slotID, ShaderType shaderType, ShaderParameterType parameterType)
{
	
}