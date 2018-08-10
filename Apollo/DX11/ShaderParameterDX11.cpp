#include "stdafx.h"
#include "ShaderParameterDX11.h"
#include "RendererDX11.h"
#include "LogManager.h"

using namespace Apollo;
using namespace std;

ShaderParameterDX11::ShaderParameterDX11()
	: m_slotID(UINT_MAX)
	, m_parameterType(ShaderParameterType::Invalid)
	, m_texture2dDX11(nullptr)
{}

ShaderParameterDX11::ShaderParameterDX11(const std::string& name, UINT slotID, ShaderType shaderType, ShaderParameterType parameterType)
	: m_Name(name)
	, m_slotID(slotID)
	, m_shaderType(shaderType)
	, m_parameterType(parameterType)
	, m_texture2dDX11(nullptr)
{}

void ShaderParameterDX11::setStructuredBuffer(StructuredBufferDX11Ptr buffer)
{
	m_structuredBufferPtr = buffer;


	//if (m_parameterType == ShaderParameterType::Buffer)
	//{
	//	// For structured buffers, DXGI_FORMAT_UNKNOWN must be used!
	//	// For standard buffers, utilize the appropriate format, 
	//	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	//	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	//	srvDesc.ViewDimension = D3D_SRV_DIMENSION_BUFFER;
	//	/*
	//	typedef struct D3D11_BUFFER_SRV {
	//		union {
	//			UINT FirstElement;
	//			UINT ElementOffset;
	//		};
	//		union {
	//			UINT NumElements;
	//			UINT ElementWidth;
	//		};
	//		上面是union，所以只能设置一个
	//	};
	//	*/
	//	//srvDesc.Buffer.ElementOffset = 0;
	//	srvDesc.Buffer.NumElements = buffer->m_elementCount;
	//	//srvDesc.Buffer.ElementWidth = buffer->m_elementSize;
	//	srvDesc.Buffer.FirstElement = 0;
	//	HRESULT hr = RendererDX11::getInstance().getDevice()->CreateShaderResourceView(m_structuredBufferPtr->m_bufferComPtr.Get(), 
	//																																		&srvDesc, m_shaderResourceViewPtr.GetAddressOf());
	//	if (hr != S_OK)
	//	{
	//		LogManager::getInstance().log("StructuredBufferDX11->CreateShaderResourceView falise!");
	//	}
	//}
	//else
	//{
	//	//create uav
	//	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	//	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	//	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	//	uavDesc.Buffer.FirstElement = 0;
	//	uavDesc.Buffer.NumElements = buffer->m_elementCount;;
	//	uavDesc.Buffer.Flags = 0;
	//	if (m_parameterType == ShaderParameterType::AppendBuffer)
	//		uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;
	//	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	//	HRESULT hr = RendererDX11::getInstance().getDevice()->CreateUnorderedAccessView(	m_structuredBufferPtr->m_bufferComPtr.Get(),
	//																													&uavDesc, m_unorderedAccessViewPtr.GetAddressOf());

	//	if (hr != S_OK)
	//	{
	//		LogManager::getInstance().log("StructuredBufferDX11->CreateUnorderedAccessView falise!");
	//	}
	//}
}

void ShaderParameterDX11::setTexture2d(Texture2dDX11* tex2d)
{
	m_texture2dDX11 = tex2d;
}

void ShaderParameterDX11::bind()
{
	if (m_texture2dDX11)
		m_texture2dDX11->bind(m_slotID, m_shaderType, m_parameterType);	
		

	if (m_constantBufferPtr)
		m_constantBufferPtr->bind(m_slotID, m_shaderType, m_parameterType);

	if (m_structuredBufferPtr)
	{
		m_structuredBufferPtr->bind(m_slotID, m_shaderType, m_parameterType);		
	}		
}

void ShaderParameterDX11::unBind()
{
	if (m_texture2dDX11)
	{
		m_texture2dDX11->unBind(m_slotID, m_shaderType, m_parameterType);
	}		

	if (m_constantBufferPtr)
		m_constantBufferPtr->unBind(m_slotID, m_shaderType, m_parameterType);

	if (m_structuredBufferPtr)
	{
		m_structuredBufferPtr->unBind(m_slotID, m_shaderType, m_parameterType);
	}
}