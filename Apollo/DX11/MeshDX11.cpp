#include "stdafx.h"
#include "MeshDX11.h"
#include "RendererDX11.h"
#include "LogManager.h"
#include "Graphics/SDKMesh.h"
#include "MaterialDX11.h"
#include "TextureDX11ResourceFactory.h"
#include "CharacterTools.h"

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

void MeshDX11::createFromSDKMeshFile(SDKMesh& sdkMesh, uint32 meshIdx)
{
	//
	const SDKMESH_MESH& sdkMeshData = *sdkMesh.GetMesh(meshIdx);

	uint32 indexSize = 2;
	m_indexType = DXGI_FORMAT_R16_UINT;
	if (sdkMesh.GetIndexType(meshIdx) == IT_32BIT)
	{
		indexSize = 4;
		m_indexType = DXGI_FORMAT_R32_UINT;
	}

	m_strideSize = sdkMesh.GetVertexStride(meshIdx, 0);
	m_vertexCount = static_cast<uint32>(sdkMesh.GetNumVertices(meshIdx, 0));
	m_indexCount = static_cast<uint32>(sdkMesh.GetNumIndices(meshIdx));
	const uint32 vbIdx = sdkMeshData.VertexBuffers[0];
	const uint32 ibIdx = sdkMeshData.IndexBuffer;

	//CreateInputElements(sdkMesh.VBElements(0));

	/*vertices.resize(vertexStride * numVertices, 0);
	memcpy(vertices.data(), sdkMesh.GetRawVerticesAt(vbIdx), vertexStride * numVertices);

	indices.resize(indexSize * numIndices, 0);
	memcpy(indices.data(), sdkMesh.GetRawIndicesAt(ibIdx), indexSize * numIndices);

	if (generateTangents)
	GenerateTangentFrame();*/

	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	bufferDesc.ByteWidth = m_strideSize * m_vertexCount;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = sdkMesh.GetRawVerticesAt(vbIdx);// vertices.data();
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;
	//DXCall(device->CreateBuffer(&bufferDesc, &initData, &vertexBuffer));
	HRESULT hr = RendererDX11::getInstance().getDevice()->CreateBuffer(&bufferDesc, &initData, m_vertexBufferPtr.GetAddressOf());
	if (hr != S_OK)
		LogManager::getInstance().log("SubMeshDX11::init Create VertexBuffer Failse!");

	bufferDesc.ByteWidth = indexSize * m_indexCount;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	initData.pSysMem = sdkMesh.GetRawIndicesAt(ibIdx);// indices.data();
													  //DXCall(device->CreateBuffer(&bufferDesc, &initData, &indexBuffer));
	hr = RendererDX11::getInstance().getDevice()->CreateBuffer(&bufferDesc, &initData, m_indexBufferPtr.GetAddressOf());
	if (hr != S_OK)
		LogManager::getInstance().log("SubMeshDX11::init Create IndexBuffer Failse!");

	const uint32 numSubsets = sdkMesh.GetNumSubsets(meshIdx);
	m_subMeshList.resize(numSubsets);
	for (uint32 i = 0; i < numSubsets; ++i)
	{
		const SDKMESH_SUBSET& subset = *sdkMesh.GetSubset(meshIdx, i);
		SubMeshDX11& subMesh = m_subMeshList[i];
		subMesh.m_indexStart = static_cast<uint32>(subset.IndexStart);
		subMesh.m_indexCount = static_cast<uint32>(subset.IndexCount);
		subMesh.m_vertexStart = static_cast<uint32>(subset.VertexStart);
		subMesh.m_vertexCount = static_cast<uint32>(subset.VertexCount);
		subMesh.m_subMeshID = i;
		//subMesh.m_materialID = subset.MaterialID;
		subMesh.m_parent = this;
		subMesh.setMaterialID(subset.MaterialID);
	}
}

SubMeshDX11* MeshDX11::createFromMemory(void* vertexBuffer, int vertexSize, uint32_t vertexCount, void* indexBuffer,
															uint32_t indexCount, DXGI_FORMAT type /* = DXGI_FORMAT_R16_UINT */)
{
	//create vertex buffer
	m_vertexCount = vertexCount;
	m_strideSize = vertexSize;
	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = vertexCount * vertexSize;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = 0;// D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA subData;
	subData.pSysMem = vertexBuffer;
	subData.SysMemPitch = 0;
	subData.SysMemSlicePitch = 0;
	HRESULT hr = RendererDX11::getInstance().getDevice()->CreateBuffer(&desc, &subData, m_vertexBufferPtr.GetAddressOf());
	if (hr != S_OK)
	{
		LogManager::getInstance().log("Create VertexBuffer Failse!");
	}

	//create index buffer
	m_indexType = type;
	m_indexCount = indexCount;
	//D3D11_BUFFER_DESC desc;
	if(m_indexType == DXGI_FORMAT_R16_UINT)
		desc.ByteWidth = indexCount * 2;
	else 
		desc.ByteWidth = indexCount * 4;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.CPUAccessFlags = 0;// D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;

//	D3D11_SUBRESOURCE_DATA subData;
	subData.pSysMem = indexBuffer;
	subData.SysMemPitch = 0;
	subData.SysMemSlicePitch = 0;
	hr = RendererDX11::getInstance().getDevice()->CreateBuffer(&desc, &subData, m_indexBufferPtr.GetAddressOf());
	if (hr != S_OK)
	{
		LogManager::getInstance().log("Create IndexBuffer False!");
	}

	SubMeshDX11 subMesh;
	subMesh.m_indexCount = indexCount;
	subMesh.m_indexStart = 0;
	subMesh.m_vertexCount = vertexCount;
	subMesh.m_vertexStart = 0;
	subMesh.m_materialID = 0;
	subMesh.m_subMeshID = 0;
	subMesh.m_parent = this;
	m_subMeshList.push_back(subMesh);
	return &m_subMeshList[m_subMeshList.size() - 1];
}

void MeshDX11::bind()const
{
	RendererDX11::getInstance().getDeviceContex()->IASetVertexBuffers(0, 1, m_vertexBufferPtr.GetAddressOf(), &m_strideSize, &m_vertexBufferOffset);
	RendererDX11::getInstance().getDeviceContex()->IASetIndexBuffer(m_indexBufferPtr.Get(), m_indexType, 0);
	RendererDX11::getInstance().getDeviceContex()->IASetPrimitiveTopology(m_ePrimType);
}

void MeshDX11::drawSubMesh(uint32_t subMeshID)const
{
	if (subMeshID >= m_subMeshList.size())
		return;

	bind();
	const SubMeshDX11& subMesh = m_subMeshList[subMeshID];
	RendererDX11::getInstance().drawIndexed(subMesh.m_indexCount, subMesh.m_indexStart, subMesh.m_vertexStart);	
}