#include "stdafx.h"
#include "Texture2dDX11.h"
#include "RendererDX11.h"

using namespace Apollo;

Texture2dDX11::Texture2dDX11(const std::string& name) : TextureResource(name, 0, TextureType_2D)
{
	ZeroMemory(&m_tex2dDesc, sizeof(m_tex2dDesc));
}

Texture2dDX11::Texture2dDX11(const std::string& path,
	uint32_t handle) :
	TextureResource(path, handle, TextureType_2D)
{
	ZeroMemory(&m_tex2dDesc, sizeof(m_tex2dDesc));
}

Texture2dDX11::Texture2dDX11(	const std::string& path, 
													uint32_t handle, 
													D3D11_TEXTURE2D_DESC desc,
													ID3D11Texture2D* texture2d,
													DepthStencilViewComPtr dsv,
													RenderTargetViewComPtr rtv,
													ShaderResourceViewComPtr srv,
													UnorderedAccessViewComPtr uva):
													TextureResource(path, handle, TextureType_2D),		
													m_tex2dDesc(desc),
													m_tex2dDx11(texture2d),
													m_depthStencilViewPtr(dsv),
													m_renderTargetViewPtr(rtv),
													m_shaderResourceViewPtr(srv),
													m_unorderedAccessViewPtr(uva)
{

}

Texture2dDX11::~Texture2dDX11()
{
	int ii = 0;
}

void Texture2dDX11::clear(ClearFlags clearFlags,const Vector4& color /* = Vector4::ZERO */, float depth /* = 1.0f */, uint8_t stencil /* = 0 */)
{
	if (m_renderTargetViewPtr)
	{
		RendererDX11::getInstance().getDeviceContex()->ClearRenderTargetView(m_renderTargetViewPtr.Get(), (float*)&color);
	}

	if (m_depthStencilViewPtr)
	{
		UINT flags = 0;
		flags |= ((int)clearFlags & (int)ClearFlags::Depth) != 0 ? D3D11_CLEAR_DEPTH : 0;
		flags |= ((int)clearFlags & (int)ClearFlags::Stencil) != 0 ? D3D11_CLEAR_STENCIL : 0;
		RendererDX11::getInstance().getDeviceContex()->ClearDepthStencilView(m_depthStencilViewPtr.Get(), flags, depth, stencil);
	}
}

void Texture2dDX11::bind(UINT slotID, ShaderType shaderType, ShaderParameterType parameterType)
{
	if (m_tex2dDx11)
	{
		if (parameterType == ShaderParameterType::Texture && m_shaderResourceViewPtr)
		{
			switch (shaderType)
			{
			case ShaderType::VertexShader:
				RendererDX11::getInstance().getDeviceContex()->VSSetShaderResources(slotID, 1, m_shaderResourceViewPtr.GetAddressOf());
				break;
			case ShaderType::TessellationControlShader:
				RendererDX11::getInstance().getDeviceContex()->HSSetShaderResources(slotID, 1, m_shaderResourceViewPtr.GetAddressOf());
				break;
			case ShaderType::TessellationEvaluationShader:
				RendererDX11::getInstance().getDeviceContex()->DSSetShaderResources(slotID, 1, m_shaderResourceViewPtr.GetAddressOf());
				break;
			case ShaderType::GeometryShader:
				RendererDX11::getInstance().getDeviceContex()->GSSetShaderResources(slotID, 1, m_shaderResourceViewPtr.GetAddressOf());
				break;
			case ShaderType::PixelShader:
				RendererDX11::getInstance().getDeviceContex()->PSSetShaderResources(slotID, 1, m_shaderResourceViewPtr.GetAddressOf());
				break;
			case ShaderType::ComputeShader:
				RendererDX11::getInstance().getDeviceContex()->CSSetShaderResources(slotID, 1, m_shaderResourceViewPtr.GetAddressOf());
				break;
			}
		}
		else if (parameterType == ShaderParameterType::RWTexture && m_unorderedAccessViewPtr)
		{
			switch (shaderType)
			{
			case ShaderType::ComputeShader:
				RendererDX11::getInstance().getDeviceContex()->CSSetUnorderedAccessViews(slotID, 1, m_unorderedAccessViewPtr.GetAddressOf(), nullptr);
				break;
			}
		}
	}	
}

void Texture2dDX11::unBind(UINT slotID, ShaderType shaderType, ShaderParameterType parameterType)
{
	if (m_tex2dDx11)
	{
		ID3D11ShaderResourceView* srv[] = { nullptr };
		ID3D11UnorderedAccessView* uav[] = { nullptr };

		if (parameterType == ShaderParameterType::Texture && m_shaderResourceViewPtr)
		{
			switch (shaderType)
			{
			case ShaderType::VertexShader:
				RendererDX11::getInstance().getDeviceContex()->VSSetShaderResources(slotID, 1, srv);
				break;
			case ShaderType::TessellationControlShader:
				RendererDX11::getInstance().getDeviceContex()->HSSetShaderResources(slotID, 1, srv);
				break;
			case ShaderType::TessellationEvaluationShader:
				RendererDX11::getInstance().getDeviceContex()->DSSetShaderResources(slotID, 1, srv);
				break;
			case ShaderType::GeometryShader:
				RendererDX11::getInstance().getDeviceContex()->GSSetShaderResources(slotID, 1, srv);
				break;
			case ShaderType::PixelShader:
				RendererDX11::getInstance().getDeviceContex()->PSSetShaderResources(slotID, 1, srv);
				break;
			case ShaderType::ComputeShader:
				RendererDX11::getInstance().getDeviceContex()->CSSetShaderResources(slotID, 1, srv);
				break;
			}
		}
		else if (parameterType == ShaderParameterType::RWTexture && m_unorderedAccessViewPtr)
		{
			switch (shaderType)
			{
			case ShaderType::ComputeShader:
				RendererDX11::getInstance().getDeviceContex()->CSSetUnorderedAccessViews(slotID, 1, uav, nullptr);
				break;
			}
		}
	}	
}

void Texture2dDX11::setRendertargetView(ID3D11RenderTargetView* rttView)
{
	m_renderTargetViewPtr = rttView;
}