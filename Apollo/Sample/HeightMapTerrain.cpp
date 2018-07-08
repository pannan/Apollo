#include "stdafx.h"
#include "HeightMapTerrain.h"
#include "VertexStruct.h"
#include "RendererDX11.h"
#include "TextureDX11ResourceFactory.h"
#include "Texture2dDX11.h"
#include "EventManager.h"
#include "Matrix3f.h"

using namespace DirectX;
using namespace  Apollo;

struct TriangleChunk
{
	uint32_t index[3];
	Vector3 normal;
};

struct ShareVertex
{
	Vector3f	normal;
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
	if (EventManager::getInstancePtr())
	{
		EventManager::getInstance().removeMouseEventListener(this);
	}
	SAFE_DELETE(m_camera);
	SAFE_DELETE_ARRAY(m_terrainPosBuffer);
	SAFE_DELETE_ARRAY(m_terrainIndexBuffer);
}

void HeightMapTerrain::init()
{
	m_camera = new Camera(Vector3(400, 100, -150), Vector3(0, 0, 0), Vector3(0, 1, 0), 1, 5000, 89);
	m_camera->setViewportWidth(1280);
	m_camera->setViewportHeight(800);

	EventManager::getInstance().addMouseEventListener(this);

	m_terrainSize = 128;//dds纹理size必须为4的倍数，这里改成128
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
	m_terrainPosBuffer = new Vector3f[m_vertexCount];
	m_terrainIndexBuffer = new uint32_t[m_indexCount];
	for (int z = 0; z < m_terrainSize; ++z)
	{
		for (int x = 0; x < m_terrainSize; ++x)
		{
			data[z * m_terrainSize + x].pos = Vector3f(x, 0, z);
			data[z * m_terrainSize + x].uv0 = Vector2f(x / float(m_terrainSize - 1), z / float(m_terrainSize - 1));
			m_terrainPosBuffer[z * m_terrainSize + x] = Vector3f(x, 0, z);
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
	Vector3f* normalBuffer = new Vector3f[m_vertexCount];

	computeNormal((byte*)data, m_terrainIndexBuffer, sizeof(Vertex_Pos_UV0), 0, m_vertexCount, m_indexCount, normalBuffer);

	m_terrainMesh = MeshDX11Ptr(new MeshDX11);

	
	m_terrainMesh->createVertexBuffer(data, sizeof(Vertex_Pos_UV0), m_vertexCount * sizeof(Vertex_Pos_UV0), m_vertexCount);
	m_terrainMesh->createIndexBuffer(m_terrainIndexBuffer, sizeof(uint32_t), m_indexCount * sizeof(uint32_t), m_indexCount, DXGI_FORMAT_R32_UINT);


	SAFE_DELETE_ARRAY(data);
	//SAFE_DELETE_ARRAY(meshIndex);
}

//现在这里有问题，缺少高度信息，因为高度是在vs里获取
//应该可以在cs里计算normnal到一个uva buffer里
void HeightMapTerrain::computeNormal(	byte* vertexBuffer,uint32_t* indexBuffer,int vertexSize,int positionOffset,int vertexCount,
																	uint32_t indexCount,Vector3f* outNormalBuffer)
{
	struct Triangle 
	{		
		uint32_t index[3];
		Vector3f normal;
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
		Vector3f* p0 = (Vector3f*)((byte*)vertexBuffer + vertexSize * index0 + positionOffset);
		Vector3f* p1 = (Vector3f*)((byte*)vertexBuffer + vertexSize * index1 + positionOffset);
		Vector3f* p2 = (Vector3f*)((byte*)vertexBuffer + vertexSize * index2 + positionOffset);

		Vector3f vec0 = *p1 - *p0;
		Vector3f vec1 = *p2 - *p0;
		Vector3f normal = vec0.Cross(vec1);
		normal.Normalize();

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
		outNormalBuffer[i].Normalize();
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
	m_terrainVertexStructBuffer = StructuredBufferDX11Ptr(new StructuredBufferDX11(m_vertexCount, sizeof(Vector3f), false, false, &subData));

	subData.pSysMem = m_terrainIndexBuffer;
	subData.SysMemPitch = 0;
	subData.SysMemSlicePitch = 0;
	m_terrainIndexStructBuffer = StructuredBufferDX11Ptr(new StructuredBufferDX11(m_indexCount, sizeof(uint32_t), false, false, &subData));

	//分配临时数据
	uint32_t triangleCount = m_indexCount / 3;
	TriangleChunk* triangleBuffer = new TriangleChunk[triangleCount];
	ZeroMemory(triangleBuffer, sizeof(TriangleChunk) * triangleCount);

	subData.pSysMem = triangleBuffer;
	subData.SysMemPitch = 0;
	subData.SysMemSlicePitch = 0;
	m_TriangleRWStructBuffer = StructuredBufferDX11Ptr(new StructuredBufferDX11(triangleCount, sizeof(TriangleChunk), false, true, &subData));

	m_shareVertexRWStructBuffer = StructuredBufferDX11Ptr(new StructuredBufferDX11(m_vertexCount, sizeof(ShareVertex), false, true, nullptr));

	m_vertexNormalRWStructBuffer = StructuredBufferDX11Ptr(new StructuredBufferDX11(m_vertexCount, sizeof(Vector3f), false, true, nullptr));

	SAFE_DELETE_ARRAY(triangleBuffer);


	////////////////////////////创建cs计算normal需要的shader//////////////////////////////////////////////

	m_vsShader = ShaderDX11Ptr(new ShaderDX11());
	m_vsShader->loadShaderFromFile(VertexShader,
		"../bin/Assets/Shader/HeightTerrain.hlsl",
		ShaderMacros(),
		"VSMAIN",
		"vs_5_0");

	m_vsShader->setConstantBuffer("PerObject", m_mvpBuffer);
	m_vsShader->setTexture2d("HeightMap", heightMapTex);
	m_vsShader->setStructuredBuffer("VertexNormalBuffer",m_vertexNormalRWStructBuffer);

	m_psShader = ShaderDX11Ptr(new ShaderDX11());
	m_psShader->loadShaderFromFile(PixelShader,
		"../bin/Assets/Shader/HeightTerrain.hlsl",
		ShaderMacros(),
		"PSMAIN",
		"ps_5_0");

	//////////////////////////////cs shader////////////////////////////////////////////

	m_computerTriangleNormalShader = ShaderDX11Ptr(new ShaderDX11());
	m_computerTriangleNormalShader->loadShaderFromFile(ComputeShader,
		"../bin/Assets/Shader/HeightMapNormalCS.hlsl",
		ShaderMacros(),
		"CS_ComputeTriangleNormal",
		"cs_5_0");

	m_computerTriangleNormalShader->setTexture2d("heightmap", heightMapTex);
	m_computerTriangleNormalShader->setStructuredBuffer("TerrainVertexBuffer", m_terrainVertexStructBuffer);
	m_computerTriangleNormalShader->setStructuredBuffer("IndexBuffer", m_terrainIndexStructBuffer);
	m_computerTriangleNormalShader->setStructuredBuffer("TriangleBuffer", m_TriangleRWStructBuffer);

	m_computerShareVertexNormal = ShaderDX11Ptr(new ShaderDX11());
	m_computerShareVertexNormal->loadShaderFromFile(ComputeShader,
		"../bin/Assets/Shader/HeightMapNormalCS.hlsl",
		ShaderMacros(),
		"CS_ComputeShareVertex",
		"cs_5_0");

	m_computerShareVertexNormal->setStructuredBuffer("TriangleBuffer", m_TriangleRWStructBuffer);
	m_computerShareVertexNormal->setStructuredBuffer("ShareVertexBuffer", m_shareVertexRWStructBuffer);

	m_computerVertexNormal = ShaderDX11Ptr(new ShaderDX11());
	m_computerVertexNormal->loadShaderFromFile(ComputeShader,
		"../bin/Assets/Shader/HeightMapNormalCS.hlsl",
		ShaderMacros(),
		"CS_ComputeVertexNormal",
		"cs_5_0");

	m_computerVertexNormal->setStructuredBuffer("ShareVertexBuffer", m_shareVertexRWStructBuffer);
	m_computerVertexNormal->setStructuredBuffer("VertexNormalBuffer", m_vertexNormalRWStructBuffer);

	
}

void HeightMapTerrain::computeNormalWithGPU()
{
	m_computerTriangleNormalShader->bin();
	RendererDX11::getInstance().getDeviceContex()->Dispatch(127, 127, 1);
	m_computerTriangleNormalShader->unBin();

	m_computerShareVertexNormal->bin();
	RendererDX11::getInstance().getDeviceContex()->Dispatch(127, 127, 1);
	m_computerShareVertexNormal->unBin();

	m_computerVertexNormal->bin();
	RendererDX11::getInstance().getDeviceContex()->Dispatch(16, 16, 1);
	m_computerVertexNormal->unBin();
}

void HeightMapTerrain::render()
{
	computeNormalWithGPU();

	m_camera->updateViewProjMatrix();
	m_mvpBuffer->set(m_camera->getViewProjMat().m_matrix, sizeof(Matrix4x4));
	m_vsShader->bin();
	m_psShader->bin();
	m_renderState.setRenderState(RendererDX11::getInstance().getDeviceContex());

	m_terrainMesh->draw();

	m_vsShader->unBin();
	m_psShader->unBin();
}

void HeightMapTerrain::onMouseMoveEvent(MouseEventArg* arg)
{
	if (arg->rButton == false)
		return;
	Vector2f currentMousePos = Vector2f(arg->mouseX, arg->mouseY);
	Vector2f dxdy = currentMousePos - m_lastMousePos;

	Matrix3f mat;
	dxdy /= 180.0f;
	if (abs(dxdy.x) > abs(dxdy.y))
	{				
		mat.RotationEuler(Vector3f(0, 1, 0), dxdy.x);
	}
	else
	{
		mat.RotationEuler(Vector3f(1, 0, 0), dxdy.y);
	}

	XMMATRIX matxx = XMMatrixRotationRollPitchYaw(dxdy.y, dxdy.x, 0);

	Vector3 camDir = m_camera->getDirection();
	Vector3f camDir3f(camDir.m_x, camDir.m_y, camDir.m_z);
	Vector3f newDir = mat * camDir3f;
	m_camera->setDirection(newDir.x, newDir.y, newDir.z);

	m_lastMousePos = currentMousePos;
}