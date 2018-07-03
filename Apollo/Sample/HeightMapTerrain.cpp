#include "stdafx.h"
#include "HeightMapTerrain.h"
#include "VertexStruct.h"
#include "RenderStateDX11.h"
#include "RendererDX11.h"
#include "TextureDX11ResourceFactory.h"
#include "Texture2dDX11.h"
using namespace  Apollo;

HeightMapTerrain::HeightMapTerrain()
{
	m_camera = nullptr;
	m_heightMapTex = nullptr;
}

HeightMapTerrain::~HeightMapTerrain()
{
	SAFE_DELETE(m_camera);
}

void HeightMapTerrain::init()
{
	m_camera = new Camera(Vector3(400, 100, -150), Vector3(0, 0, 0), Vector3(0, 1, 0), 1, 5000, 90);
	m_camera->setViewportWidth(1280);
	m_camera->setViewportHeight(800);
	m_terrainSize = 129;
	createMesh();

	createShader();
}

void HeightMapTerrain::createMesh()
{
	uint32_t vertexCount = m_terrainSize * m_terrainSize;
	uint32_t indexCount = (m_terrainSize - 1) * (m_terrainSize - 1) * 6;
	Vertex_Pos_UV0* data = new Vertex_Pos_UV0[vertexCount];

	for (int z = 0; z < m_terrainSize; ++z)
	{
		for (int x = 0; x < m_terrainSize; ++x)
		{
			data[z * m_terrainSize + x].pos = Vector3f(x, 0, z);
			data[z * m_terrainSize + x].uv0 = Vector2f(x / float(m_terrainSize - 1), z / float(m_terrainSize - 1));
		}
	}

	uint32_t* meshIndex = new uint32_t[indexCount];

	for (int z = 0; z < m_terrainSize - 1; ++z)
	{
		for (int x = 0; x < m_terrainSize - 1; ++x)
		{
			uint32_t vertexIndex = z * m_terrainSize + x;
			uint32_t quadIndex = z * (m_terrainSize - 1) + x;
			uint32_t baseIndex = quadIndex * 6;		//每个quad两个triangles，6个index

			meshIndex[baseIndex] = z * m_terrainSize + x;
			meshIndex[baseIndex + 1] = (z + 1) * m_terrainSize + x;
			meshIndex[baseIndex + 2] = (z + 1) * m_terrainSize + x + 1;

			meshIndex[baseIndex + 3] = (z + 1) * m_terrainSize + x + 1;
			meshIndex[baseIndex + 4] = z * m_terrainSize + x + 1;
			meshIndex[baseIndex + 5] = z * m_terrainSize + x;
		}
	}

	m_terrainMesh = MeshDX11Ptr(new MeshDX11);

	
	m_terrainMesh->createVertexBuffer(data, sizeof(Vertex_Pos_UV0), vertexCount * sizeof(Vertex_Pos_UV0), vertexCount);
	m_terrainMesh->createIndexBuffer(meshIndex, sizeof(uint32_t), indexCount * sizeof(uint32_t), indexCount, DXGI_FORMAT_R32_UINT);


	SAFE_DELETE_ARRAY(data);
	SAFE_DELETE_ARRAY(meshIndex);
}

void HeightMapTerrain::createShader()
{
	m_mvpBuffer = ConstantBufferDX11Ptr(new ConstantBufferDX11(sizeof(Matrix4x4), true, true, nullptr));

	uint32_t texHandle = TextureDX11ResourceFactory::getInstance().createResource("..\\bin\\Assets\\Texture\\heightmap.dds", "heightmap.dds", "dds");
	m_heightMapTex = (Texture2dDX11*)TextureDX11ResourceFactory::getInstance().getResource(texHandle);

	m_vsShader = ShaderDX11Ptr(new ShaderDX11());
	m_vsShader->loadShaderFromFile(VertexShader,
		"../bin/Assets/Shader/HeightTerrain.hlsl",
		ShaderMacros(),
		"VSMAIN",
		"vs_5_0");

	m_vsShader->setConstantBuffer("PerObject", m_mvpBuffer);
	//这里设置现在无效
	m_vsShader->setTexture2d("HeightMap", m_heightMapTex);

	m_psShader = ShaderDX11Ptr(new ShaderDX11());
	m_psShader->loadShaderFromFile(PixelShader,
		"../bin/Assets/Shader/HeightTerrain.hlsl",
		ShaderMacros(),
		"PSMAIN",
		"ps_5_0");

	
}

void HeightMapTerrain::render()
{
	m_camera->updateViewProjMatrix();
	m_mvpBuffer->set(m_camera->getViewProjMat().m_matrix, sizeof(Matrix4x4));
	m_vsShader->bin();
	m_psShader->bin();
	RenderStateDX11::getInstance().setDefaultRenderState(RendererDX11::getInstance().getDeviceContex());

	//现在貌似shader反射检测不到在vs里使用纹理，这种手动设置下
	RendererDX11::getInstance().getDeviceContex()->VSSetShaderResources(0, 1, m_heightMapTex->getSRVPtr());
	m_terrainMesh->draw();

	m_vsShader->unBin();
	m_psShader->unBin();
}