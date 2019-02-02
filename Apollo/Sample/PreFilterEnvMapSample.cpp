#include "stdafx.h"
#include "TextureDX11ResourceFactory.h"
#include "RendererDX11.h"
#include "Texture2dDX11.h"
#include "VertexStruct.h"
#include "MaterialDX11.h"
#include "PreFilterEnvMapSample.h"

NAME_SPACE_BEGIN_APOLLO

PreFilterEnvMapSample::PreFilterEnvMapSample()
{

}

PreFilterEnvMapSample::~PreFilterEnvMapSample()
{

}

void PreFilterEnvMapSample::init()
{
	m_srcTextureHandle = TextureDX11ResourceFactory::getInstance().createResource("..\\bin\\Assets\\Texture\\panorama.hdr", "panorama.hdr", "hdr");
	Texture2dDX11* srcTex2d = (Texture2dDX11*)TextureDX11ResourceFactory::getInstance().getResource(m_srcTextureHandle);
}

void PreFilterEnvMapSample::initQuadMesh()
{
	//m_quadMesh = MeshDX11Ptr(new MeshDX11);

	Vertex_Pos_UV0 data[4];
	data[0].pos = Vector3(-0.5, 0.5, 0);
	data[0].uv0 = Vector2(0, 0);
	data[1].pos = Vector3(0.5, 0.5, 0);
	data[1].uv0 = Vector2(1, 0);
	data[2].pos = Vector3(0.5, -0.5, 0);
	data[2].uv0 = Vector2(1, 1);
	data[3].pos = Vector3(-0.5, -0.5, 0);
	data[3].uv0 = Vector2(0, 1);

	//m_quadMesh->createVertexBuffer(data, sizeof(Vertex_Pos_UV0), 4 * sizeof(Vertex_Pos_UV0),4);

	uint16_t index[6] = { 0,1,2,2,3,0 };
	//m_quadMesh->createIndexBuffer(index, sizeof(uint16_t), 6 * sizeof(uint16_t),6);
	//m_quadMesh->createFromMemory(data, sizeof(Vertex_Pos_UV0), 4, index, 6);
	m_quadModel.createFromMemory(data, sizeof(Vertex_Pos_UV0), 4, index, 6);
}

NAME_SPACE_END