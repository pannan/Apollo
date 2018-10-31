#include "stdafx.h"
#include "Texture3dDX11.h"
#include "RendererDX11.h"

NAME_SPACE_BEGIN_APOLLO

Texture3dDX11::Texture3dDX11(const std::string& name) : TextureResource(name, 0, TextureType_3D)
{
	ZeroMemory(&m_tex3dDesc, sizeof(m_tex3dDesc));
}

Texture3dDX11::Texture3dDX11(const std::string& path,
	uint32_t handle) :
	TextureResource(path, handle, TextureType_3D)
{
	ZeroMemory(&m_tex3dDesc, sizeof(m_tex3dDesc));
}

Texture3dDX11::Texture3dDX11(const std::string& path,
	uint32_t handle,
	D3D11_TEXTURE3D_DESC desc,
	ID3D11Texture3D* texture3d,
	ShaderResourceViewComPtr srv) :
	TextureResource(path, handle, TextureType_3D),
	m_tex3dDesc(desc),
	m_tex3dDx11(texture3d),
	m_shaderResourceViewPtr(srv)
{
}

Texture3dDX11::~Texture3dDX11()
{
	int ii = 0;
}

void Texture3dDX11::bind(UINT slotID, ShaderType shaderType, ShaderParameterType parameterType)
{
	if (m_tex3dDx11)
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
		/*else if (parameterType == ShaderParameterType::RWTexture && m_unorderedAccessViewPtr)
		{
			switch (shaderType)
			{
			case ShaderType::ComputeShader:
				RendererDX11::getInstance().getDeviceContex()->CSSetUnorderedAccessViews(slotID, 1, m_unorderedAccessViewPtr.GetAddressOf(), nullptr);
				break;
			}
		}*/
	}
}

void Texture3dDX11::unBind(UINT slotID, ShaderType shaderType, ShaderParameterType parameterType)
{
	if (m_tex3dDx11)
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
		/*else if (parameterType == ShaderParameterType::RWTexture && m_unorderedAccessViewPtr)
		{
			switch (shaderType)
			{
			case ShaderType::ComputeShader:
				RendererDX11::getInstance().getDeviceContex()->CSSetUnorderedAccessViews(slotID, 1, uav, nullptr);
				break;
			}
		}*/
	}
}


NAME_SPACE_END