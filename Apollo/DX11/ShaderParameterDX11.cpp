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


	if (m_parameterType == ShaderParameterType::Buffer)
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
		srvDesc.Buffer.NumElements = buffer->m_elementCount;
		//srvDesc.Buffer.ElementWidth = buffer->m_elementSize;
		srvDesc.Buffer.FirstElement = 0;
		HRESULT hr = RendererDX11::getInstance().getDevice()->CreateShaderResourceView(m_structuredBufferPtr->m_bufferComPtr.Get(), 
																																			&srvDesc, m_shaderResourceViewPtr.GetAddressOf());
		if (hr != S_OK)
		{
			LogManager::getInstance().log("StructuredBufferDX11->CreateShaderResourceView falise!");
		}
	}
	else
	{
		//create uav
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
		uavDesc.Format = DXGI_FORMAT_UNKNOWN;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.NumElements = buffer->m_elementCount;;
		uavDesc.Buffer.Flags = 0;
		if (m_parameterType == ShaderParameterType::AppendBuffer)
			uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		HRESULT hr = RendererDX11::getInstance().getDevice()->CreateUnorderedAccessView(	m_structuredBufferPtr->m_bufferComPtr.Get(),
																														&uavDesc, m_unorderedAccessViewPtr.GetAddressOf());

		if (hr != S_OK)
		{
			LogManager::getInstance().log("StructuredBufferDX11->CreateUnorderedAccessView falise!");
		}
	}
}

void ShaderParameterDX11::setTexture2d(Texture2dDX11* tex2d)
{
	m_texture2dDX11 = tex2d;

	if (m_parameterType == ShaderParameterType::Texture)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC resourceViewDesc;
		resourceViewDesc.Format = tex2d->m_tex2dDesc.Format;

		if (tex2d->m_tex2dDesc.ArraySize > 1)
		{
			if (tex2d->m_tex2dDesc.SampleDesc.Count > 1)
			{
				resourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY;
				resourceViewDesc.Texture2DMSArray.FirstArraySlice = 0;
				resourceViewDesc.Texture2DMSArray.ArraySize = tex2d->m_tex2dDesc.ArraySize;
			}
			else
			{
				resourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
				resourceViewDesc.Texture2DArray.FirstArraySlice = 0;
				resourceViewDesc.Texture2DArray.ArraySize = tex2d->m_tex2dDesc.ArraySize;
				resourceViewDesc.Texture2DArray.MipLevels = tex2d->m_tex2dDesc.MipLevels;
				resourceViewDesc.Texture2DArray.MostDetailedMip = 0;
			}
		}
		else
		{
			if (tex2d->m_tex2dDesc.SampleDesc.Count > 1)
			{
				resourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
			}
			else
			{
				resourceViewDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2D;
				resourceViewDesc.Texture2D.MipLevels = tex2d->m_tex2dDesc.MipLevels;
				resourceViewDesc.Texture2D.MostDetailedMip = 0;
			}
		}

		if (FAILED(RendererDX11::getInstance().getDevice()->CreateShaderResourceView(tex2d->m_tex2dDx11.Get(),
			&resourceViewDesc, m_shaderResourceViewPtr.GetAddressOf())))
		{
			LogManager::getInstance().log("Failed to create texture resource view.");
		}
		else if (tex2d->m_tex2dDesc.MipLevels == 0)
		{
			RendererDX11::getInstance().getDeviceContex()->GenerateMips(m_shaderResourceViewPtr.Get());
		}		
	}
	else if (m_parameterType == ShaderParameterType::RWTexture)
	{
		assert(tex2d->m_tex2dDesc.SampleDesc.Count == 1);

		// Create a Shader resource view for the texture.
		D3D11_UNORDERED_ACCESS_VIEW_DESC unorderedAccessViewDesc;
		unorderedAccessViewDesc.Format = tex2d->m_tex2dDesc.Format;

		if (tex2d->m_tex2dDesc.ArraySize > 1)
		{
			unorderedAccessViewDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
			unorderedAccessViewDesc.Texture2DArray.MipSlice = 0;
			unorderedAccessViewDesc.Texture2DArray.FirstArraySlice = 0;
			unorderedAccessViewDesc.Texture2DArray.ArraySize = tex2d->m_tex2dDesc.ArraySize;
		}
		else
		{
			unorderedAccessViewDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
			unorderedAccessViewDesc.Texture2D.MipSlice = 0;
		}

		if (FAILED(RendererDX11::getInstance().getDevice()->CreateUnorderedAccessView(	tex2d->m_tex2dDx11.Get(), 
																																		&unorderedAccessViewDesc, m_unorderedAccessViewPtr.GetAddressOf())))
		{
			LogManager::getInstance().log("Failed to create unordered access view.");
		}
	}
	else
	{
		LogManager::getInstance().log("setTexture2d Parameter type error!");
	}
}

void ShaderParameterDX11::bind()
{
	if (m_texture2dDX11)
	{
		//m_texture2dDX11->bind(m_slotID, m_shaderType, m_parameterType);
		if (m_parameterType == ShaderParameterType::Texture && m_shaderResourceViewPtr)
		{
			switch (m_shaderType)
			{
			case ShaderType::VertexShader:
				RendererDX11::getInstance().getDeviceContex()->VSSetShaderResources(m_slotID, 1, m_shaderResourceViewPtr.GetAddressOf());
				break;
			case ShaderType::TessellationControlShader:
				RendererDX11::getInstance().getDeviceContex()->HSSetShaderResources(m_slotID, 1, m_shaderResourceViewPtr.GetAddressOf());
				break;
			case ShaderType::TessellationEvaluationShader:
				RendererDX11::getInstance().getDeviceContex()->DSSetShaderResources(m_slotID, 1, m_shaderResourceViewPtr.GetAddressOf());
				break;
			case ShaderType::GeometryShader:
				RendererDX11::getInstance().getDeviceContex()->GSSetShaderResources(m_slotID, 1, m_shaderResourceViewPtr.GetAddressOf());
				break;
			case ShaderType::PixelShader:
				RendererDX11::getInstance().getDeviceContex()->PSSetShaderResources(m_slotID, 1, m_shaderResourceViewPtr.GetAddressOf());
				break;
			case ShaderType::ComputeShader:
				RendererDX11::getInstance().getDeviceContex()->CSSetShaderResources(m_slotID, 1, m_shaderResourceViewPtr.GetAddressOf());
				break;
			}
		}
		else if (m_parameterType== ShaderParameterType::RWTexture && m_unorderedAccessViewPtr)
		{
			switch (m_shaderType)
			{
			case ShaderType::ComputeShader:
				RendererDX11::getInstance().getDeviceContex()->CSSetUnorderedAccessViews(m_slotID, 1, m_unorderedAccessViewPtr.GetAddressOf(), nullptr);
				break;
			}
		}
	}
		

	if (m_constantBufferPtr)
		m_constantBufferPtr->bind(m_slotID, m_shaderType, m_parameterType);

	if (m_structuredBufferPtr)
	{
//m_structuredBufferPtr->bind(m_slotID, m_shaderType, m_parameterType);
		if (m_parameterType == ShaderParameterType::Buffer && m_shaderResourceViewPtr)
		{
			ID3D11ShaderResourceView* srv[] = { m_shaderResourceViewPtr.Get() };

			switch (m_shaderType)
			{
			case ShaderType::VertexShader:
				RendererDX11::getInstance().getDeviceContex()->VSSetShaderResources(m_slotID, 1, srv);
				break;
			case ShaderType::TessellationControlShader:
				RendererDX11::getInstance().getDeviceContex()->HSSetShaderResources(m_slotID, 1, srv);
				break;
			case ShaderType::TessellationEvaluationShader:
				RendererDX11::getInstance().getDeviceContex()->DSSetShaderResources(m_slotID, 1, srv);
				break;
			case ShaderType::GeometryShader:
				RendererDX11::getInstance().getDeviceContex()->GSSetShaderResources(m_slotID, 1, srv);
				break;
			case ShaderType::PixelShader:
				RendererDX11::getInstance().getDeviceContex()->PSSetShaderResources(m_slotID, 1, srv);
				break;
			case ShaderType::ComputeShader:
				RendererDX11::getInstance().getDeviceContex()->CSSetShaderResources(m_slotID, 1, srv);
				break;
			}
		}
		else if((m_parameterType == ShaderParameterType::AppendBuffer || m_parameterType == ShaderParameterType::RWBuffer) && m_unorderedAccessViewPtr)
		{
			ID3D11UnorderedAccessView* uav[] = { m_unorderedAccessViewPtr.Get() };
			switch (m_shaderType)
			{
			case ShaderType::ComputeShader:
				RendererDX11::getInstance().getDeviceContex()->CSSetUnorderedAccessViews(m_slotID, 1, uav, nullptr);
				break;
			}
		}
	}
		
}

void ShaderParameterDX11::unBind()
{
	if (m_texture2dDX11)
	{
		ID3D11ShaderResourceView* srv[] = { nullptr };
		ID3D11UnorderedAccessView* uav[] = { nullptr };

		if (m_parameterType == ShaderParameterType::Texture && m_shaderResourceViewPtr)
		{
			switch (m_shaderType)
			{
			case ShaderType::VertexShader:
				RendererDX11::getInstance().getDeviceContex()->VSSetShaderResources(m_slotID, 1, srv);
				break;
			case ShaderType::TessellationControlShader:
				RendererDX11::getInstance().getDeviceContex()->HSSetShaderResources(m_slotID, 1, srv);
				break;
			case ShaderType::TessellationEvaluationShader:
				RendererDX11::getInstance().getDeviceContex()->DSSetShaderResources(m_slotID, 1, srv);
				break;
			case ShaderType::GeometryShader:
				RendererDX11::getInstance().getDeviceContex()->GSSetShaderResources(m_slotID, 1, srv);
				break;
			case ShaderType::PixelShader:
				RendererDX11::getInstance().getDeviceContex()->PSSetShaderResources(m_slotID, 1, srv);
				break;
			case ShaderType::ComputeShader:
				RendererDX11::getInstance().getDeviceContex()->CSSetShaderResources(m_slotID, 1, srv);
				break;
			}
		}
		else if (m_parameterType == ShaderParameterType::RWTexture && m_unorderedAccessViewPtr)
		{
			switch (m_shaderType)
			{
			case ShaderType::ComputeShader:
				RendererDX11::getInstance().getDeviceContex()->CSSetUnorderedAccessViews(m_slotID, 1, uav, nullptr);
				break;
			}
		}
	}		

	if (m_constantBufferPtr)
		m_constantBufferPtr->unBind(m_slotID, m_shaderType, m_parameterType);

	if (m_structuredBufferPtr)
	{
		ID3D11UnorderedAccessView* uav[] = { nullptr };
		ID3D11ShaderResourceView* srv[] = { nullptr };

		if (m_parameterType == ShaderParameterType::Buffer)
		{
			switch (m_shaderType)
			{
			case ShaderType::VertexShader:
				RendererDX11::getInstance().getDeviceContex()->VSSetShaderResources(m_slotID, 1, srv);
				break;
			case ShaderType::TessellationControlShader:
				RendererDX11::getInstance().getDeviceContex()->HSSetShaderResources(m_slotID, 1, srv);
				break;
			case ShaderType::TessellationEvaluationShader:
				RendererDX11::getInstance().getDeviceContex()->DSSetShaderResources(m_slotID, 1, srv);
				break;
			case ShaderType::GeometryShader:
				RendererDX11::getInstance().getDeviceContex()->GSSetShaderResources(m_slotID, 1, srv);
				break;
			case ShaderType::PixelShader:
				RendererDX11::getInstance().getDeviceContex()->PSSetShaderResources(m_slotID, 1, srv);
				break;
			case ShaderType::ComputeShader:
				RendererDX11::getInstance().getDeviceContex()->CSSetShaderResources(m_slotID, 1, srv);
				break;
			}
		}
		else if (m_parameterType == ShaderParameterType::RWBuffer)
		{
			switch (m_shaderType)
			{
			case ShaderType::ComputeShader:
				RendererDX11::getInstance().getDeviceContex()->CSSetUnorderedAccessViews(m_slotID, 1, uav, nullptr);
				break;
			}
		}
	}
}