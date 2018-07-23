#include "stdafx.h"
#include "MeshDX11.h"
#include "RendererDX11.h"
#include "LogManager.h"
#include "Graphics/SDKMesh.h"
#include "MaterialDX11.h"
using namespace Apollo;

MeshDX11::MeshDX11()
{
	m_strideSize = 0;
	m_vertexBufferOffset = 0;
	m_ePrimType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_vertexCount = m_indexCount = 0;
	m_indexType = DXGI_FORMAT_R16_UINT;
}

MeshDX11::~MeshDX11()
{

}

void MeshDX11::createFromSDKMeshFile(LPCWSTR fileName)
{
	SDKMesh sdkMesh;
	sdkMesh.Create(fileName);

	uint32 numMaterials = sdkMesh.GetNumMaterials();
	for (uint32 i = 0; i < numMaterials; ++i)
	{
		MaterialDX11 material;
		SDKMESH_MATERIAL* mat = sdkMesh.GetMaterial(i);
		
		material.m_albedoMap = mat->DiffuseTexture;// AnsiToWString(mat->DiffuseTexture);
		material.m_normalMap = mat->NormalTexture;// AnsiToWString(mat->NormalTexture);

		//LoadMaterialResources(material, directory, device);

		//meshMaterials.push_back(material);
	}
}

void MeshDX11::createVertexBuffer(void* data, int vertexSize, uint32_t buffSize,uint32_t vertexCount)
{
	m_vertexCount = vertexCount;
	m_strideSize = vertexSize;
	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = buffSize;
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

void MeshDX11::createIndexBuffer(void* data, int vertexSize, uint32_t buffSize,uint32_t indexCount, DXGI_FORMAT type)
{
	m_indexType = type;
	m_indexCount = indexCount;
	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = buffSize;
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
	RendererDX11::getInstance().getDeviceContex()->IASetVertexBuffers(0, 1, m_vertexBufferPtr.GetAddressOf(), &m_strideSize, &m_vertexBufferOffset);
	RendererDX11::getInstance().getDeviceContex()->IASetIndexBuffer(m_indexBufferPtr.Get(), m_indexType, 0);
	RendererDX11::getInstance().getDeviceContex()->IASetPrimitiveTopology(m_ePrimType);
	RendererDX11::getInstance().getDeviceContex()->DrawIndexed(m_indexCount, 0, 0);
}