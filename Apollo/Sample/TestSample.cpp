#include "stdafx.h"
#include "TestSample.h"
#include "TextureDX11ResourceFactory.h"
#include "RendererDX11.h"
#include "Texture2dDX11.h"
#include "VertexStruct.h"
#include "MaterialDX11.h"
using namespace Apollo;

TestSample::TestSample()
{
	m_colorBuffer = nullptr;
}

TestSample::~TestSample()
{
	SAFE_DELETE_ARRAY(m_colorBuffer);
}

void TestSample::init()
{
	m_terrainSize = 128;

	m_colorBuffer = new Vector3[m_terrainSize * m_terrainSize];

	for (int z = 0; z < m_terrainSize; ++z)
	{
		for (int x = 0; x < m_terrainSize; ++x)
		{
			float u = (float)x / (m_terrainSize - 1);
			float v = (float)z / (m_terrainSize - 1);
			m_colorBuffer[z * m_terrainSize + x] = Vector3(u, v, 0);
		}
	}

	D3D11_SUBRESOURCE_DATA subData;
	subData.pSysMem = m_colorBuffer;
	subData.SysMemPitch = 0;
	subData.SysMemSlicePitch = 0;
	m_ColorStructBuffer = StructuredBufferDX11Ptr(new StructuredBufferDX11(m_terrainSize * m_terrainSize, sizeof(Vector3), false, false, false,&subData));

	//create texture
	Texture2dConfigDX11 tex2dConfig;
	tex2dConfig.SetWidth(m_terrainSize);
	tex2dConfig.SetHeight(m_terrainSize);
	tex2dConfig.SetBindFlags(D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS);

	//创建cs写入的uav
	uint32_t texHandle = TextureDX11ResourceFactory::getInstance().createTexture2D("CS_Texture_Process", tex2dConfig);

	Texture2dDX11* uavTex = (Texture2dDX11*)TextureDX11ResourceFactory::getInstance().getResource(texHandle);

	//create shader
	m_vsShader = ShaderDX11Ptr(new ShaderDX11());
	m_vsShader->loadShaderFromFile(ShaderType::VertexShader,
		"../bin/Assets/Shader/TextureVS.hlsl",
		ShaderMacros(),
		"VSMAIN",
		"vs_5_0");

	m_psShader = ShaderDX11Ptr(new ShaderDX11());
	m_psShader->loadShaderFromFile(ShaderType::PixelShader,
		"../bin/Assets/Shader/TexturePS.hlsl",
		ShaderMacros(),
		"PSMAIN",
		"ps_5_0");

	m_psShader->setTexture2d("ColorMap00", uavTex);
	MaterialDX11* material = new MaterialDX11;
	material->m_vs = m_vsShader;
	material->m_ps[(uint8_t)RenderPipelineType::ForwardRender] = m_psShader;
	m_quadModel.addMaterial(MaterialPtr(material));

	m_computerFetchColorToTextureShader = ShaderDX11Ptr(new ShaderDX11());
	m_computerFetchColorToTextureShader->loadShaderFromFile(ShaderType::ComputeShader,
		"../bin/Assets/Shader/Test_CS.hlsl",
		ShaderMacros(),
		"CS_Color",
		"cs_5_0");

	m_computerFetchColorToTextureShader->setStructuredBuffer("TerrainVertexBuffer", m_ColorStructBuffer);
	m_computerFetchColorToTextureShader->setTexture2d("OutputMap", uavTex);

	m_renderState.createState();

	initQuadMesh();
}

void TestSample::initQuadMesh()
{
	//m_quadMesh = MeshDX11Ptr(new MeshDX11);

	Vertex_Pos_UV0 data[4];
	data[0].pos = Vector3(-1, 1, 0);
	data[0].uv0 = Vector2(0, 0);
	data[1].pos = Vector3(1, 1, 0);
	data[1].uv0 = Vector2(1, 0);
	data[2].pos = Vector3(1, -1, 0);
	data[2].uv0 = Vector2(1, 1);
	data[3].pos = Vector3(-1, -1, 0);
	data[3].uv0 = Vector2(0, 1);

	//m_quadMesh->createVertexBuffer(data, sizeof(Vertex_Pos_UV0), 4 * sizeof(Vertex_Pos_UV0), 4);

	uint16_t index[6] = { 0,1,2,2,3,0 };
	//m_quadMesh->createIndexBuffer(index, sizeof(uint16_t), 6 * sizeof(uint16_t), 6);
	m_quadModel.createFromMemory(data, sizeof(Vertex_Pos_UV0), 4, index, 6);
}

void TestSample::render()
{
	//m_ColorStructBuffer->commit(m_colorBuffer, m_terrainSize * m_terrainSize * sizeof(Vector3));
	m_computerFetchColorToTextureShader->bin();

	//测试纹理为256x256，先硬编码
	RendererDX11::getInstance().getDeviceContex()->Dispatch(16, 16, 1);

	m_computerFetchColorToTextureShader->unBind();

	//draw tex
	//RenderStateDX11::getInstance().setRenderState();
	m_renderState.bind();
	//m_vsShader->bin();
	//m_psShader->bin();
	float bf[4] = { 1,1,1,1 };
	//RendererDX11::getInstance().getDeviceContex()->OMSetBlendState(m_blendState, bf, 0xffffffff);
	//RendererDX11::getInstance().getDeviceContex()->IASetInputLayout(g_pInputLayout);
	//m_quadMesh->draw();
	m_quadModel.draw();

	//m_vsShader->unBind();
	//m_psShader->unBind();
}