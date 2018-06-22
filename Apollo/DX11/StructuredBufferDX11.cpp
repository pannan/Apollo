#include "stdafx.h"
#include "StructuredBufferDX11.h"
#include "RendererDX11.h"

using namespace Apollo;

StructuredBufferDX11::StructuredBufferDX11(UINT count, UINT structsize, bool CPUWritable, bool GPUWritable, D3D11_SUBRESOURCE_DATA* pData)
{
	m_bUAV = (CPUWritable && !GPUWritable);
	m_bufferComPtr = RendererDX11::getInstance().createStructuredBuffer(count, structsize, CPUWritable, GPUWritable, pData);

	//create uav
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements = count;
	uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	RendererDX11::getInstance().getDevice()->CreateUnorderedAccessView(m_bufferComPtr.Get(), &uavDesc, m_uavComPtr.GetAddressOf());

	//create srv
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Buffer.ElementOffset = 0;
	srvDesc.Buffer.NumElements = count;
	srvDesc.Buffer.ElementWidth = structsize;
	srvDesc.Buffer.FirstElement = 0;
	RendererDX11::getInstance().getDevice()->CreateShaderResourceView(m_bufferComPtr.Get(), &srvDesc, m_srcComPtr.GetAddressOf());
}