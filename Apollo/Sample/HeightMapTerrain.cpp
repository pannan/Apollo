#include "stdafx.h"
#include "HeightMapTerrain.h"
#include "VertexStruct.h"
#include "RendererDX11.h"
#include "TextureDX11ResourceFactory.h"
#include "Texture2dDX11.h"
#include "Graphics/Camera.h"
#include "Timer.h"

using namespace DirectX;
using namespace  Apollo;

struct TriangleChunk
{
	uint32_t index[3];
	Vector3 normal;
};

struct ShareVertex
{
	Vector3	normal;
	uint32_t   shareCount;
};

HeightMapTerrain::HeightMapTerrain()
{
	m_camera = nullptr;
	m_terrainPosBuffer = nullptr;
	m_terrainIndexBuffer = nullptr;
}

HeightMapTerrain::~HeightMapTerrain()
{
	SAFE_DELETE(m_camera);
	SAFE_DELETE_ARRAY(m_terrainPosBuffer);
	SAFE_DELETE_ARRAY(m_terrainIndexBuffer);
}

void HeightMapTerrain::init()
{
	m_camera = new Camera(Vector3(400, 100, -150), Vector3(0, 0, 0), Vector3(0, 1, 0), 0.001, 5000, 90 * _PI / 180.0f);
	m_camera->setViewportWidth(1280);
	m_camera->setViewportHeight(800);
	//m_camera = new FirstPersonCamera(1280.0f / 800.0f, Pi_4 * 0.75f, 0.01, 10000);
	///m_camera->SetLookAt(Float3(400, 100, -150), Float3(0, 0, 0), Float3(0, 1, 0));

	m_terrainSize = 1024;//dds纹理size必须为4的倍数，这里改成128
	createMesh();

	createShader();

	m_renderState.m_depthStencilDesc.DepthEnable = true;
	m_renderState.createState();
}

void HeightMapTerrain::createMesh()
{
	m_vertexCount = m_terrainSize * m_terrainSize;
	m_indexCount = (m_terrainSize - 1) * (m_terrainSize - 1) * 6;
	Vertex_Pos_UV0* data = new Vertex_Pos_UV0[m_vertexCount];
	m_terrainPosBuffer = new Vector3[m_vertexCount];
	m_terrainIndexBuffer = new uint32_t[m_indexCount];
	for (int z = 0; z < m_terrainSize; ++z)
	{
		for (int x = 0; x < m_terrainSize; ++x)
		{
			data[z * m_terrainSize + x].pos = Vector3(x, 0, z);
			data[z * m_terrainSize + x].uv0 = Vector2(x / float(m_terrainSize - 1), z / float(m_terrainSize - 1));
			m_terrainPosBuffer[z * m_terrainSize + x] = Vector3(x, 0, z);
		}
	}

	//uint32_t* meshIndex = new uint32_t[m_indexCount];

	for (int z = 0; z < m_terrainSize - 1; ++z)
	{
		for (int x = 0; x < m_terrainSize - 1; ++x)
		{
			uint32_t vertexIndex = z * m_terrainSize + x;
			uint32_t quadIndex = z * (m_terrainSize - 1) + x;
			uint32_t baseIndex = quadIndex * 6;		//每个quad两个triangles，6个index

			m_terrainIndexBuffer[baseIndex] = z * m_terrainSize + x;
			m_terrainIndexBuffer[baseIndex + 1] = (z + 1) * m_terrainSize + x;
			m_terrainIndexBuffer[baseIndex + 2] = (z + 1) * m_terrainSize + x + 1;

			m_terrainIndexBuffer[baseIndex + 3] = (z + 1) * m_terrainSize + x + 1;
			m_terrainIndexBuffer[baseIndex + 4] = z * m_terrainSize + x + 1;
			m_terrainIndexBuffer[baseIndex + 5] = z * m_terrainSize + x;
		}
	}

	//计算法线
	//Vector3* normalBuffer = new Vector3[m_vertexCount];

	//computeNormal((byte*)data, m_terrainIndexBuffer, sizeof(Vertex_Pos_UV0), 0, m_vertexCount, m_indexCount, normalBuffer);

	//m_terrainMesh = MeshDX11Ptr(new MeshDX11);

	
	//m_terrainMesh->createVertexBuffer(data, sizeof(Vertex_Pos_UV0), m_vertexCount * sizeof(Vertex_Pos_UV0), m_vertexCount);
	//m_terrainMesh->createIndexBuffer(m_terrainIndexBuffer, sizeof(uint32_t), m_indexCount * sizeof(uint32_t), m_indexCount, DXGI_FORMAT_R32_UINT);
	m_terrainModel.createFromMemory(data, sizeof(Vertex_Pos_UV0), m_vertexCount, m_terrainIndexBuffer, m_indexCount, DXGI_FORMAT_R32_UINT);

	SAFE_DELETE_ARRAY(data);
	//SAFE_DELETE_ARRAY(meshIndex);
}

//现在这里有问题，缺少高度信息，因为高度是在vs里获取
//应该可以在cs里计算normnal到一个uva buffer里
void HeightMapTerrain::computeNormal(	byte* vertexBuffer,uint32_t* indexBuffer,int vertexSize,int positionOffset,int vertexCount,
																	uint32_t indexCount,Vector3* outNormalBuffer)
{
	struct Triangle 
	{		
		uint32_t index[3];
		Vector3 normal;
	};

	uint32_t triangleCount = indexCount / 3;
	Triangle* triangleList = new Triangle[triangleCount];

	for (uint32_t i = 0; i < indexCount; i +=3)
	{
		uint32_t triangleIndex = i / 3;
		uint32_t index0 = indexBuffer[i];
		uint32_t index1 = indexBuffer[i + 1];
		uint32_t index2 = indexBuffer[i + 2];

		//计算法线
		Vector3* p0 = (Vector3*)((byte*)vertexBuffer + vertexSize * index0 + positionOffset);
		Vector3* p1 = (Vector3*)((byte*)vertexBuffer + vertexSize * index1 + positionOffset);
		Vector3* p2 = (Vector3*)((byte*)vertexBuffer + vertexSize * index2 + positionOffset);

		Vector3 vec0 = *p1 - *p0;
		Vector3 vec1 = *p2 - *p0;
		Vector3 normal = vec0.corss(vec1);
		normal.normalize();

		triangleList[triangleIndex].normal = normal;

		triangleList[triangleIndex].index[0] = index0;
		triangleList[triangleIndex].index[1] = index1;
		triangleList[triangleIndex].index[2] = index2;
	}

	ComputeNormalChunk* tempBuffer = new ComputeNormalChunk[vertexCount];
	ZeroMemory(tempBuffer, sizeof(ComputeNormalChunk) * vertexCount);
	for (uint32_t triangleIndex = 0; triangleIndex < triangleCount; ++triangleIndex)
	{
		const Triangle& triangle = triangleList[triangleIndex];

		for (int i = 0; i < 3; ++i)
		{
			uint32_t vertexIndex = triangle.index[i];
			tempBuffer[vertexIndex].normal += triangle.normal;
			++tempBuffer[vertexIndex].shareCount;
		}
	}

	//平均normal
	for (int i = 0; i < vertexCount; ++i)
	{
		outNormalBuffer[i] = tempBuffer[i].normal / (float)tempBuffer[i].shareCount;
		outNormalBuffer[i].normalize();
	}

	SAFE_DELETE_ARRAY(tempBuffer);
}

void HeightMapTerrain::createShader()
{
	m_mvpBuffer = ConstantBufferDX11Ptr(new ConstantBufferDX11(sizeof(Matrix4x4), true, true, nullptr));

	uint32_t texHandle = TextureDX11ResourceFactory::getInstance().createResource("..\\bin\\Assets\\Texture\\heightmap.dds", "heightmap.dds", "dds");
	Texture2dDX11* heightMapTex = (Texture2dDX11*)TextureDX11ResourceFactory::getInstance().getResource(texHandle);

	////////////////////////////创建cs计算normal需要的buffer资源//////////////////////////////////////////////

	D3D11_SUBRESOURCE_DATA subData;
	subData.pSysMem = m_terrainPosBuffer;
	subData.SysMemPitch = 0;
	subData.SysMemSlicePitch = 0;
	m_terrainVertexStructBuffer = StructuredBufferDX11Ptr(new StructuredBufferDX11(m_vertexCount, sizeof(Vector3), false, false, false,&subData));

	subData.pSysMem = m_terrainIndexBuffer;
	subData.SysMemPitch = 0;
	subData.SysMemSlicePitch = 0;
	m_terrainIndexStructBuffer = StructuredBufferDX11Ptr(new StructuredBufferDX11(m_indexCount, sizeof(uint32_t), false, false, false,&subData));

	//分配临时数据
	uint32_t triangleCount = m_indexCount / 3;
	TriangleChunk* triangleBuffer = new TriangleChunk[triangleCount];
	ZeroMemory(triangleBuffer, sizeof(TriangleChunk) * triangleCount);

	subData.pSysMem = triangleBuffer;
	subData.SysMemPitch = 0;
	subData.SysMemSlicePitch = 0;
	m_TriangleRWStructBuffer = StructuredBufferDX11Ptr(new StructuredBufferDX11(triangleCount, sizeof(TriangleChunk), false, true, false,&subData));

	m_shareVertexRWStructBuffer = StructuredBufferDX11Ptr(new StructuredBufferDX11(m_vertexCount, sizeof(ShareVertex), false, true, false,nullptr));

	m_vertexNormalRWStructBuffer = StructuredBufferDX11Ptr(new StructuredBufferDX11(m_vertexCount, sizeof(Vector3), false, true, false, nullptr));

	SAFE_DELETE_ARRAY(triangleBuffer);


	////////////////////////////创建cs计算normal需要的shader//////////////////////////////////////////////

	//render shader
	m_vsShader = ShaderDX11Ptr(new ShaderDX11());
	m_vsShader->loadShaderFromFile(ShaderType::VertexShader,
		"../bin/Assets/Shader/HeightTerrain.hlsl",
		ShaderMacros(),
		"VSMAIN",
		"vs_5_0");

	m_vsShader->setConstantBuffer("PerObject", m_mvpBuffer);
	m_vsShader->setTexture2d("HeightMap", heightMapTex);
	m_vsShader->setStructuredBuffer("VertexNormalBuffer",m_vertexNormalRWStructBuffer);

	m_psShader = ShaderDX11Ptr(new ShaderDX11());
	m_psShader->loadShaderFromFile(ShaderType::PixelShader,
		"../bin/Assets/Shader/HeightTerrain.hlsl",
		ShaderMacros(),
		"PSMAIN",
		"ps_5_0");

	MaterialDX11* material = new MaterialDX11;
	material->m_vs = m_vsShader;
	material->m_ps[(uint8_t)RenderPipelineType::ForwardRender] = m_psShader;
	m_terrainModel.addMaterial(MaterialPtr(material));

	//////////////////////////////cs shader////////////////////////////////////////////

	m_computerTriangleNormalShader = ShaderDX11Ptr(new ShaderDX11());
	m_computerTriangleNormalShader->loadShaderFromFile(ShaderType::ComputeShader,
		"../bin/Assets/Shader/HeightMapNormalCS.hlsl",
		ShaderMacros(),
		"CS_ComputeTriangleNormal",
		"cs_5_0");

	m_computerTriangleNormalShader->setTexture2d("HeightMap", heightMapTex);
	m_computerTriangleNormalShader->setStructuredBuffer("TerrainVertexBuffer", m_terrainVertexStructBuffer);
	m_computerTriangleNormalShader->setStructuredBuffer("IndexBuffer", m_terrainIndexStructBuffer);
	m_computerTriangleNormalShader->setStructuredBuffer("TriangleBuffer", m_TriangleRWStructBuffer);

	m_initShareVertexShader = ShaderDX11Ptr(new ShaderDX11());
	m_initShareVertexShader->loadShaderFromFile(ShaderType::ComputeShader,
		"../bin/Assets/Shader/HeightMapNormalCS.hlsl",
		ShaderMacros(),
		"CS_InitShareVertex",
		"cs_5_0");

	m_initShareVertexShader->setStructuredBuffer("ShareVertexBuffer", m_shareVertexRWStructBuffer);

	m_computerShareVertexNormal = ShaderDX11Ptr(new ShaderDX11());
	m_computerShareVertexNormal->loadShaderFromFile(ShaderType::ComputeShader,
		"../bin/Assets/Shader/HeightMapNormalCS.hlsl",
		ShaderMacros(),
		"CS_ComputeShareVertex",
		"cs_5_0");

	m_computerShareVertexNormal->setStructuredBuffer("TriangleBuffer", m_TriangleRWStructBuffer);
	m_computerShareVertexNormal->setStructuredBuffer("ShareVertexBuffer", m_shareVertexRWStructBuffer);

	m_computerVertexNormal = ShaderDX11Ptr(new ShaderDX11());
	m_computerVertexNormal->loadShaderFromFile(ShaderType::ComputeShader,
		"../bin/Assets/Shader/HeightMapNormalCS.hlsl",
		ShaderMacros(),
		"CS_ComputeVertexNormal",
		"cs_5_0");

	m_computerVertexNormal->setStructuredBuffer("ShareVertexBuffer", m_shareVertexRWStructBuffer);
	m_computerVertexNormal->setStructuredBuffer("VertexNormalBuffer", m_vertexNormalRWStructBuffer);

	computeNormalWithGPU();
}

void HeightMapTerrain::computeNormalWithGPU()
{
	m_computerTriangleNormalShader->bin();
	RendererDX11::getInstance().getDeviceContex()->Dispatch(1023, 1023, 1);
	m_computerTriangleNormalShader->unBind();

	m_initShareVertexShader->bin();
	RendererDX11::getInstance().getDeviceContex()->Dispatch(32, 32, 1);
	m_initShareVertexShader->unBind();

	m_computerShareVertexNormal->bin();
	RendererDX11::getInstance().getDeviceContex()->Dispatch(1023, 1023, 1);
	m_computerShareVertexNormal->unBind();

	m_computerVertexNormal->bin();
	RendererDX11::getInstance().getDeviceContex()->Dispatch(32, 32, 1);
	m_computerVertexNormal->unBind();
}

void HeightMapTerrain::updateCamera()
{
	/*const float FilmSize = 35.0f * 0.001f;
	const float FocalLength = 35.0f * 0.001f;
	const float aspectRatio = m_camera->AspectRatio();
	float verticalSize = FilmSize / aspectRatio;
	const float VerticalFOV = 2.0f * std::atan2(verticalSize, 2.0f * FocalLength);

	m_camera->SetFieldOfView(VerticalFOV);*/
}

void HeightMapTerrain::render()
{
	updateCamera();
	//computeNormalWithGPU();

	m_camera->updateViewProjMatrix();
	//m_mvpBuffer->set(m_camera->ViewProjectionMatrix().m, sizeof(Float4x4));
	m_mvpBuffer->set(m_camera->getViewProjMat().m_matrix, sizeof(Matrix4x4));
	//m_vsShader->bin();
	//m_psShader->bin();
	m_renderState.bind();

	m_terrainModel.draw();

	//m_vsShader->unBind();
	//m_psShader->unBind();
}