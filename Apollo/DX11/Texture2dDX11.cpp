#include "stdafx.h"
#include "Texture2dDX11.h"
#include "RendererDX11.h"

using namespace Apollo;

Texture2dDX11::Texture2dDX11(const std::string& path,
	uint32_t handle,
	ShaderResourceViewComPtr srv) :
	TextureResource(path, handle, TextureType_2D),
	m_shaderResourceViewPtr(srv)
{

}

Texture2dDX11::Texture2dDX11(const std::string& path, 
													uint32_t handle, 
													ShaderResourceViewComPtr srv,
													DepthStencilViewComPtr dsv,
													RenderTargetViewComPtr rtv,
													UnorderedAccessViewComPtr uav) :
													TextureResource(path, handle, TextureType_2D),
													m_shaderResourceViewPtr(srv),
													m_depthStencilViewPtr(dsv),
													m_renderTargetViewPtr(rtv),
													m_unorderedAccessViewPtr(uav)
{

}

Texture2dDX11::~Texture2dDX11()
{
	int ii = 0;
}

void Texture2dDX11::bind(UINT slotID, ShaderType shaderType, ShaderParameterType parameterType)
{
	ID3D11ShaderResourceView* srv[] = { m_shaderResourceViewPtr.Get() };
	ID3D11UnorderedAccessView* uav[] = { m_unorderedAccessViewPtr.Get() };

	if (parameterType == ShaderParameterType::Texture && m_shaderResourceViewPtr)
	{
		switch (shaderType)
		{
		case VertexShader:
			RendererDX11::getInstance().getDeviceContex()->VSSetShaderResources(slotID, 1, srv);
			break;
		case TessellationControlShader:
			RendererDX11::getInstance().getDeviceContex()->HSSetShaderResources(slotID, 1, srv);
			break;
		case TessellationEvaluationShader:
			RendererDX11::getInstance().getDeviceContex()->DSSetShaderResources(slotID, 1, srv);
			break;
		case GeometryShader:
			RendererDX11::getInstance().getDeviceContex()->GSSetShaderResources(slotID, 1, srv);
			break;
		case PixelShader:
			RendererDX11::getInstance().getDeviceContex()->PSSetShaderResources(slotID, 1, srv);
			break;
		case ComputeShader:
			RendererDX11::getInstance().getDeviceContex()->CSSetShaderResources(slotID, 1, srv);
			break;
		}
	}
	else if (parameterType == ShaderParameterType::RWTexture && m_unorderedAccessViewPtr)
	{
		switch (shaderType)
		{
		case ComputeShader:
			RendererDX11::getInstance().getDeviceContex()->CSSetUnorderedAccessViews(slotID, 1, uav, nullptr);
			break;
		}
	}
}

void Texture2dDX11::unBind(UINT slotID, ShaderType shaderType, ShaderParameterType parameterType)
{
	ID3D11ShaderResourceView* srv[] = { nullptr };
	ID3D11UnorderedAccessView* uav[] = { nullptr };

	if (parameterType == ShaderParameterType::Texture && m_shaderResourceViewPtr)
	{
		switch (shaderType)
		{
		case VertexShader:
			RendererDX11::getInstance().getDeviceContex()->VSSetShaderResources(slotID, 1, srv);
			break;
		case TessellationControlShader:
			RendererDX11::getInstance().getDeviceContex()->HSSetShaderResources(slotID, 1, srv);
			break;
		case TessellationEvaluationShader:
			RendererDX11::getInstance().getDeviceContex()->DSSetShaderResources(slotID, 1, srv);
			break;
		case GeometryShader:
			RendererDX11::getInstance().getDeviceContex()->GSSetShaderResources(slotID, 1, srv);
			break;
		case PixelShader:
			RendererDX11::getInstance().getDeviceContex()->PSSetShaderResources(slotID, 1, srv);
			break;
		case ComputeShader:
			RendererDX11::getInstance().getDeviceContex()->CSSetShaderResources(slotID, 1, srv);
			break;
		}
	}
	else if (parameterType == ShaderParameterType::RWTexture && m_unorderedAccessViewPtr)
	{
		switch (shaderType)
		{
		case ComputeShader:
			RendererDX11::getInstance().getDeviceContex()->CSSetUnorderedAccessViews(slotID, 1, uav, nullptr);
			break;
		}
	}
}