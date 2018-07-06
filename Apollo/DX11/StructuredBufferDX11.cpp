#include "stdafx.h"
#include "StructuredBufferDX11.h"
#include "RendererDX11.h"
#include "LogManager.h"

using namespace Apollo;

StructuredBufferDX11::StructuredBufferDX11(UINT count, UINT structsize, bool CPUWritable, bool GPUWritable, D3D11_SUBRESOURCE_DATA* pData,
	ShaderParameterType type)
{
	m_bUAV = (!CPUWritable && GPUWritable);
	m_bufferComPtr = RendererDX11::getInstance().createStructuredBuffer(count, structsize, CPUWritable, GPUWritable, pData);

	if (m_bUAV)
	{
		//create uav
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
		uavDesc.Format = DXGI_FORMAT_UNKNOWN;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.NumElements = count;
		uavDesc.Buffer.Flags = 0;
		if(type == ShaderParameterType::AppendBuffer)
			uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		RendererDX11::getInstance().getDevice()->CreateUnorderedAccessView(m_bufferComPtr.Get(), &uavDesc, m_uavComPtr.GetAddressOf());
	}	

	// For structured buffers, DXGI_FORMAT_UNKNOWN must be used!
	// For standard buffers, utilize the appropriate format, 
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.ElementOffset = 0;
	srvDesc.Buffer.NumElements = count;
	srvDesc.Buffer.ElementWidth = structsize;
	srvDesc.Buffer.FirstElement = 0;
	HRESULT hr = RendererDX11::getInstance().getDevice()->CreateShaderResourceView(m_bufferComPtr.Get(), &srvDesc, m_srcComPtr.GetAddressOf());
	if (hr != S_OK)
	{
		LogManager::getInstance().log("StructuredBufferDX11->CreateShaderResourceView falise!");
	}
}

StructuredBufferDX11::~StructuredBufferDX11()
{}

bool StructuredBufferDX11::bind(unsigned int ID, ShaderType shaderType, ShaderParameterType parameterType)
{
	if (parameterType == ShaderParameterType::Buffer && m_srcComPtr)
	{
		ID3D11ShaderResourceView* srv[] = { m_srcComPtr.Get() };

		switch (shaderType)
		{
		case VertexShader:
			RendererDX11::getInstance().getDeviceContex()->VSSetShaderResources(ID, 1, srv);
			break;
		case TessellationControlShader:
			RendererDX11::getInstance().getDeviceContex()->HSSetShaderResources(ID, 1, srv);
			break;
		case TessellationEvaluationShader:
			RendererDX11::getInstance().getDeviceContex()->DSSetShaderResources(ID, 1, srv);
			break;
		case GeometryShader:
			RendererDX11::getInstance().getDeviceContex()->GSSetShaderResources(ID, 1, srv);
			break;
		case PixelShader:
			RendererDX11::getInstance().getDeviceContex()->PSSetShaderResources(ID, 1, srv);
			break;
		case ComputeShader:
			RendererDX11::getInstance().getDeviceContex()->CSSetShaderResources(ID, 1, srv);
			break;
		}
	}
	else if (parameterType == ShaderParameterType::RWBuffer && m_uavComPtr)
	{
		ID3D11UnorderedAccessView* uav[] = { m_uavComPtr.Get() };
		switch (shaderType)
		{
		case ComputeShader:
			RendererDX11::getInstance().getDeviceContex()->CSSetUnorderedAccessViews(ID, 1, uav, nullptr);
			break;
		}
	}

	return true;
}

void StructuredBufferDX11::unBind(unsigned int ID, ShaderType shaderType, ShaderParameterType parameterType)
{
	ID3D11UnorderedAccessView* uav[] = { nullptr };
	ID3D11ShaderResourceView* srv[] = { nullptr };

	if (parameterType == ShaderParameterType::Buffer)
	{
		switch (shaderType)
		{
		case VertexShader:
			RendererDX11::getInstance().getDeviceContex()->VSSetShaderResources(ID, 1, srv);
			break;
		case TessellationControlShader:
			RendererDX11::getInstance().getDeviceContex()->HSSetShaderResources(ID, 1, srv);
			break;
		case TessellationEvaluationShader:
			RendererDX11::getInstance().getDeviceContex()->DSSetShaderResources(ID, 1, srv);
			break;
		case GeometryShader:
			RendererDX11::getInstance().getDeviceContex()->GSSetShaderResources(ID, 1, srv);
			break;
		case PixelShader:
			RendererDX11::getInstance().getDeviceContex()->PSSetShaderResources(ID, 1, srv);
			break;
		case ComputeShader:
			RendererDX11::getInstance().getDeviceContex()->CSSetShaderResources(ID, 1, srv);
			break;
		}
	}
	else if (parameterType == ShaderParameterType::RWBuffer)
	{
		switch (shaderType)
		{
		case ComputeShader:
			RendererDX11::getInstance().getDeviceContex()->CSSetUnorderedAccessViews(ID, 1, uav, nullptr);
			break;
		}
	}
}
