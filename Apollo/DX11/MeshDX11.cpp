#include "stdafx.h"
#include "MeshDX11.h"
#include "RendererDX11.h"
#include "LogManager.h"

using namespace Apollo;

MeshDX11::MeshDX11()
{
	m_strideSize = 0;
	m_vertexBufferOffset = 0;
	m_ePrimType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
}

MeshDX11::~MeshDX11()
{

}

void MeshDX11::createVertexBuffer(void* data, int vertexSize,uint32_t bufferSize)
{
	m_strideSize = vertexSize;
	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = bufferSize;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = 0;// D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA subData;
	subData.pSysMem = data;
	subData.SysMemPitch = 0;
	subData.SysMemSlicePitch = 0;
	HRESULT hr = RendererDX11::getInstance().getDevice()->CreateBuffer(&desc, &subData, m_vertexBufferPtr.GetAddressOf());
	if (hr != S_OK)
	{
		LogManager::getInstance().log("Create VertexBuffer Failse!");
	}
}

void MeshDX11::createIndexBuffer(void* data, int vertexSize, uint32_t bufferSize)
{
	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = bufferSize;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.CPUAccessFlags = 0;// D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA subData;
	subData.pSysMem = data;
	subData.SysMemPitch = 0;
	subData.SysMemSlicePitch = 0;
	HRESULT hr = RendererDX11::getInstance().getDevice()->CreateBuffer(&desc, &subData, m_indexBufferPtr.GetAddressOf());
	if (hr != S_OK)
	{
		LogManager::getInstance().log("Create IndexBuffer False!");
	}
}

void MeshDX11::draw()
{
	D3D11_RECT rect;
	rect.left = 0;
	rect.top = 0;
	rect.right = 1024;
	rect.bottom = 768;
	RendererDX11::getInstance().getDeviceContex()->RSSetScissorRects(1, &rect);
	RendererDX11::getInstance().getDeviceContex()->IASetVertexBuffers(0, 1, m_vertexBufferPtr.GetAddressOf(), &m_strideSize, &m_vertexBufferOffset);
	RendererDX11::getInstance().getDeviceContex()->IASetIndexBuffer(m_indexBufferPtr.Get(), DXGI_FORMAT_R16_UINT, 0);
	RendererDX11::getInstance().getDeviceContex()->IASetPrimitiveTopology(m_ePrimType);
	RendererDX11::getInstance().getDeviceContex()->DrawIndexed(6, 0, 0);
}