#include "stdafx.h"
#include "ConstantBufferDX11.h"
#include "RendererDX11.h"
#include "LogManager.h"
using namespace Apollo;

ConstantBufferDX11::ConstantBufferDX11(size_t size) : m_bufferSize(size)
{
	m_bufferComPtr = RendererDX11::getInstance().createConstantBuffer(size, true, true, nullptr);
}

ConstantBufferDX11::ConstantBufferDX11(const D3D11_BUFFER_DESC& bufferDesc, D3D11_SUBRESOURCE_DATA* pData)
{
	m_bufferComPtr = RendererDX11::getInstance().createConstantBuffer(bufferDesc, pData);
}

ConstantBufferDX11::ConstantBufferDX11(size_t size,bool dynamic, bool CPUupdates, D3D11_SUBRESOURCE_DATA* pData) : m_bufferSize(size)
{
	m_bufferComPtr = RendererDX11::getInstance().createConstantBuffer(size, dynamic, CPUupdates, pData);
}

ConstantBufferDX11::~ConstantBufferDX11()
{}

void ConstantBufferDX11::set(const void* data, size_t size)
{
	assert(size == m_bufferSize);

	D3D11_MAPPED_SUBRESOURCE mappedResource;

	if (FAILED(RendererDX11::getInstance().getDeviceContex()->Map(m_bufferComPtr.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		LogManager::getInstance().log("Failed to map constant buffer.");
		return;
	}

	memcpy(mappedResource.pData, data, m_bufferSize);

	RendererDX11::getInstance().getDeviceContex()->Unmap(m_bufferComPtr.Get(), 0);
}


bool ConstantBufferDX11::bind(unsigned int id, ShaderType shaderType, ShaderParameterType parameterType)
{
	bool result = true;

	ID3D11Buffer* pBuffers[] = { m_bufferComPtr.Get() };

	switch (shaderType)
	{
	case ShaderType::VertexShader:
		RendererDX11::getInstance().getDeviceContex()->VSSetConstantBuffers(id, 1, pBuffers);
		break;
	case ShaderType::TessellationControlShader:
		RendererDX11::getInstance().getDeviceContex()->HSSetConstantBuffers(id, 1, pBuffers);
		break;
	case ShaderType::TessellationEvaluationShader:
		RendererDX11::getInstance().getDeviceContex()->DSSetConstantBuffers(id, 1, pBuffers);
		break;
	case ShaderType::GeometryShader:
		RendererDX11::getInstance().getDeviceContex()->GSSetConstantBuffers(id, 1, pBuffers);
		break;
	case ShaderType::PixelShader:
		RendererDX11::getInstance().getDeviceContex()->PSSetConstantBuffers(id, 1, pBuffers);
		break;
	case ShaderType::ComputeShader:
		RendererDX11::getInstance().getDeviceContex()->CSSetConstantBuffers(id, 1, pBuffers);
		break;
	default:
		result = false;
		break;
	}

	return result;
}

void ConstantBufferDX11::unBind(unsigned int id, ShaderType shaderType, ShaderParameterType parameterType)
{
	ID3D11Buffer* pBuffers[] = { nullptr };

	switch (shaderType)
	{
	case ShaderType::VertexShader:
		RendererDX11::getInstance().getDeviceContex()->VSSetConstantBuffers(id, 1, pBuffers);
		break;
	case ShaderType::TessellationControlShader:
		RendererDX11::getInstance().getDeviceContex()->HSSetConstantBuffers(id, 1, pBuffers);
		break;
	case ShaderType::TessellationEvaluationShader:
		RendererDX11::getInstance().getDeviceContex()->DSSetConstantBuffers(id, 1, pBuffers);
		break;
	case ShaderType::GeometryShader:
		RendererDX11::getInstance().getDeviceContex()->GSSetConstantBuffers(id, 1, pBuffers);
		break;
	case ShaderType::PixelShader:
		RendererDX11::getInstance().getDeviceContex()->PSSetConstantBuffers(id, 1, pBuffers);
		break;
	case ShaderType::ComputeShader:
		RendererDX11::getInstance().getDeviceContex()->CSSetConstantBuffers(id, 1, pBuffers);
		break;
	default:
		break;
	}
}