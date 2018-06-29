#include "stdafx.h"
#include "MeshDX11.h"
#include "RendererDX11.h"

using namespace Apollo;

MeshDX11::MeshDX11()
{
	m_strideSize = 0;
	m_vertexBufferOffset = 0;
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
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA subData;
	subData.pSysMem = data;
	subData.SysMemPitch = 0;
	subData.SysMemSlicePitch = 0;
	RendererDX11::getInstance().getDevice()->CreateBuffer(&desc, &subData, m_vertexBufferPtr.GetAddressOf());
}

void MeshDX11::createIndexBuffer(void* data, int vertexSize, uint32_t bufferSize)
{
	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = bufferSize;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA subData;
	subData.pSysMem = data;
	subData.SysMemPitch = 0;
	subData.SysMemSlicePitch = 0;
	RendererDX11::getInstance().getDevice()->CreateBuffer(&desc, &subData, m_indexBufferPtr.GetAddressOf());
}

void MeshDX11::bind()
{
	RendererDX11::getInstance().getDeviceContex()->IASetVertexBuffers(0, 1, m_vertexBufferPtr.GetAddressOf(), &m_strideSize, &m_vertexBufferOffset);
	RendererDX11::getInstance().getDeviceContex()->IASetIndexBuffer(m_indexBufferPtr.Get(), DXGI_FORMAT_R16_UINT, 0);
}

void MeshDX11::draw()
{

}