#include "stdafx.h"
#include "StructuredBufferDX11.h"
#include "RendererDX11.h"
#include "LogManager.h"

using namespace Apollo;

StructuredBufferDX11::StructuredBufferDX11(UINT count, UINT structsize, bool CPUWritable, bool GPUWritable, bool isAppendBuffer,D3D11_SUBRESOURCE_DATA* pData) :
					m_elementCount(count), m_elementSize(structsize),m_CPUWritable(CPUWritable),m_isAppendBuffer(isAppendBuffer)
{
	m_bUAV = (!CPUWritable && GPUWritable);
	m_bufferComPtr = RendererDX11::getInstance().createStructuredBuffer(count, structsize, CPUWritable, GPUWritable, pData);

	//srv
	//if (m_parameterType == ShaderParameterType::Buffer)
	{
		// For structured buffers, DXGI_FORMAT_UNKNOWN must be used!
		// For standard buffers, utilize the appropriate format, 
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.ViewDimension = D3D_SRV_DIMENSION_BUFFER;
		/*
		typedef struct D3D11_BUFFER_SRV {
		union {
		UINT FirstElement;
		UINT ElementOffset;
		};
		union {
		UINT NumElements;
		UINT ElementWidth;
		};
		上面是union，所以只能设置一个
		};
		*/
		//srvDesc.Buffer.ElementOffset = 0;
		srvDesc.Buffer.NumElements = count;
		//srvDesc.Buffer.ElementWidth = buffer->m_elementSize;
		srvDesc.Buffer.FirstElement = 0;
		HRESULT hr = RendererDX11::getInstance().getDevice()->CreateShaderResourceView(m_bufferComPtr.Get(),
			&srvDesc, m_srcComPtr.GetAddressOf());
		if (hr != S_OK)
		{
			LogManager::getInstance().log("StructuredBufferDX11->CreateShaderResourceView falise!");
		}
	}

	if(m_bUAV)
	{
		//create uav
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
		uavDesc.Format = DXGI_FORMAT_UNKNOWN;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.NumElements = count;
		uavDesc.Buffer.Flags = 0;
		if (m_isAppendBuffer)
			uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		HRESULT hr = RendererDX11::getInstance().getDevice()->CreateUnorderedAccessView(m_bufferComPtr.Get(),
			&uavDesc, m_uavComPtr.GetAddressOf());

		if (hr != S_OK)
		{
			LogManager::getInstance().log("StructuredBufferDX11->CreateUnorderedAccessView falise!");
		}
	}
}

StructuredBufferDX11::~StructuredBufferDX11()
{}

bool StructuredBufferDX11::bind(unsigned int ID, ShaderType shaderType, ShaderParameterType parameterType)
{
	if (m_bufferComPtr)
	{
		if (parameterType == ShaderParameterType::Buffer && m_srcComPtr)
		{
			ID3D11ShaderResourceView* srv[] = { m_srcComPtr.Get() };

			switch (shaderType)
			{
			case ShaderType::VertexShader:
				RendererDX11::getInstance().getDeviceContex()->VSSetShaderResources(ID, 1, srv);
				break;
			case ShaderType::TessellationControlShader:
				RendererDX11::getInstance().getDeviceContex()->HSSetShaderResources(ID, 1, srv);
				break;
			case ShaderType::TessellationEvaluationShader:
				RendererDX11::getInstance().getDeviceContex()->DSSetShaderResources(ID, 1, srv);
				break;
			case ShaderType::GeometryShader:
				RendererDX11::getInstance().getDeviceContex()->GSSetShaderResources(ID, 1, srv);
				break;
			case ShaderType::PixelShader:
				RendererDX11::getInstance().getDeviceContex()->PSSetShaderResources(ID, 1, srv);
				break;
			case ShaderType::ComputeShader:
				RendererDX11::getInstance().getDeviceContex()->CSSetShaderResources(ID, 1, srv);
				break;
			}
		}
		else if ((parameterType == ShaderParameterType::AppendBuffer || parameterType == ShaderParameterType::RWBuffer) && m_uavComPtr)
		{
			ID3D11UnorderedAccessView* uav[] = { m_uavComPtr.Get() };
			switch (shaderType)
			{
			case ShaderType::ComputeShader:
				RendererDX11::getInstance().getDeviceContex()->CSSetUnorderedAccessViews(ID, 1, uav, nullptr);
				break;
			}
		}
	}
	
	return true;
}

void StructuredBufferDX11::unBind(unsigned int ID, ShaderType shaderType, ShaderParameterType parameterType)
{
	if (m_bufferComPtr)
	{
		ID3D11UnorderedAccessView* uav[] = { nullptr };
		ID3D11ShaderResourceView* srv[] = { nullptr };

		if (parameterType == ShaderParameterType::Buffer)
		{
			switch (shaderType)
			{
			case ShaderType::VertexShader:
				RendererDX11::getInstance().getDeviceContex()->VSSetShaderResources(ID, 1, srv);
				break;
			case ShaderType::TessellationControlShader:
				RendererDX11::getInstance().getDeviceContex()->HSSetShaderResources(ID, 1, srv);
				break;
			case ShaderType::TessellationEvaluationShader:
				RendererDX11::getInstance().getDeviceContex()->DSSetShaderResources(ID, 1, srv);
				break;
			case ShaderType::GeometryShader:
				RendererDX11::getInstance().getDeviceContex()->GSSetShaderResources(ID, 1, srv);
				break;
			case ShaderType::PixelShader:
				RendererDX11::getInstance().getDeviceContex()->PSSetShaderResources(ID, 1, srv);
				break;
			case ShaderType::ComputeShader:
				RendererDX11::getInstance().getDeviceContex()->CSSetShaderResources(ID, 1, srv);
				break;
			}
		}
		else if (parameterType == ShaderParameterType::RWBuffer)
		{
			switch (shaderType)
			{
			case ShaderType::ComputeShader:
				RendererDX11::getInstance().getDeviceContex()->CSSetUnorderedAccessViews(ID, 1, uav, nullptr);
				break;
			}
		}
	}
}

void StructuredBufferDX11::commit(void* data, uint32_t size)
{
	if (m_CPUWritable == false)
	{
		LogManager::getInstance().log("StructuredBufferDX11::commit m_CPUWritable == false!");
		return;
	}
	
	if (m_bufferComPtr.Get() == nullptr)
	{
		LogManager::getInstance().log("StructuredBufferDX11::commit m_bufferComPtr.Get() == nullptr!");
		return;
	}
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(RendererDX11::getInstance().getDeviceContex()->Map(m_bufferComPtr.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		LogManager::getInstance().log("StructuredBufferDX11::commit Map false!");
		return;
	}

	memcpy_s(mappedResource.pData, m_elementSize * m_elementCount, data, size);

	RendererDX11::getInstance().getDeviceContex()->Unmap(m_bufferComPtr.Get(), 0);
}