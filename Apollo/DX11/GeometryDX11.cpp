#include "stdafx.h"
#include "GeometryDX11.h"
#include "VertexStruct.h"

using namespace Apollo;

void GeometryDX11::createFullSrceenQuad()
{
	//create vertex buffer
	D3D11_BUFFER_DESC desc;
	memset(&desc, 0, sizeof(D3D11_BUFFER_DESC));
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.ByteWidth = 4 * sizeof(Vertex_Pos_UV0);
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;

	Vertex_Pos_UV0 data[4];
	data[0].pos = Vector3(-1.0f, 1.0f, 0.0f);
	data[0].uv0 = Vector2(0.0f, 1.0f);

	data[1].pos = Vector3(1.0f, 1.0f, 0.0f);
	data[1].uv0 = Vector2(1.0f, 1.0f);

	data[2].pos = Vector3(1.0f, -1.0f, 0.0f);
	data[2].uv0 = Vector2(1.0f, 0.0f);

	data[3].pos = Vector3(-1.0f, -1.0f, 0.0f);
	data[3].uv0 = Vector2(0.0f, 0.0f);
	D3D11_SUBRESOURCE_DATA subData;
	subData.pSysMem = data;
	subData.SysMemPitch = 0;
	subData.SysMemSlicePitch = 0;

	m_vertexBuffer = BufferManagerDX11::getInstance().createBuffer(&desc,&subData);

	//create index buffer
	memset(&desc, 0, sizeof(D3D11_BUFFER_DESC));
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.ByteWidth = 6 * sizeof(UINT16);
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;

	UINT16 indexBuffer[6] = { 0,1,2, 2,3,0 };
	subData;
	subData.pSysMem = indexBuffer;
	subData.SysMemPitch = 0;
	subData.SysMemSlicePitch = 0;

	m_indexBuffer = BufferManagerDX11::getInstance().createBuffer(&desc, &subData);
}