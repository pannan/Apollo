#include "stdafx.h"
#include "ComputerShaderProcessTexture.h"
#include "TextureDX11ResourceFactory.h"
#include "RendererDX11.h"
#include "Texture2dDX11.h"
#include "VertexStruct.h"


using namespace Apollo;

ComputerShaderProcessTexture::ComputerShaderProcessTexture()
{

}

ComputerShaderProcessTexture::~ComputerShaderProcessTexture()
{

}

void ComputerShaderProcessTexture::init()
{
	m_srcTextureHandle = TextureDX11ResourceFactory::getInstance().createResource("..\\bin\\Assets\\Texture\\BRDF.dds", "BRDF.dds", "dds");
	Texture2dDX11* srcTex2d = (Texture2dDX11*)TextureDX11ResourceFactory::getInstance().getResource(m_srcTextureHandle);
	//ID3D11Resource* dx11Resource = nullptr;
	//srcTex2d->getSRV()->GetResource(&dx11Resource);
	ID3D11Texture2D* tex2dDX = srcTex2d->getTexture2D();// ID3D11Texture2D*)dx11Resource;
	D3D11_TEXTURE2D_DESC texDesc;
	tex2dDX->GetDesc(&texDesc);
	Texture2dConfigDX11 tex2dConfig;
	tex2dConfig.SetWidth(texDesc.Width);
	tex2dConfig.SetHeight(texDesc.Height);
	tex2dConfig.SetBindFlags(D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS);

	//创建cs写入的uav
	m_uavTextureHandle = TextureDX11ResourceFactory::getInstance().createTexture2D("CS_Texture_Process", tex2dConfig);

	Texture2dDX11* uavTex = (Texture2dDX11*)TextureDX11ResourceFactory::getInstance().getResource(m_uavTextureHandle);

	//Texture2dDX11Ptr uavTexPtr(uavTex);
	//Texture2dDX11Ptr srcTexPtr(srcTex2d);

	m_vsShader = ShaderDX11Ptr(new ShaderDX11());
	m_vsShader->loadShaderFromFile(VertexShader,
		"../bin/Assets/Shader/TextureVS.hlsl",
		ShaderMacros(),
		"VSMAIN",
		"vs_5_0");

	m_psShader = ShaderDX11Ptr(new ShaderDX11());
	m_psShader->loadShaderFromFile(PixelShader,
		"../bin/Assets/Shader/TexturePS.hlsl",
		ShaderMacros(),
		"PSMAIN",
		"ps_5_0");

	m_psShader->setTexture2d("ColorMap00", uavTex);

	m_csShader = ShaderDX11Ptr(new ShaderDX11());
	m_csShader->loadShaderFromFile(ComputeShader,
		"../bin/Assets/Shader/InvertColorCS.hlsl",
		ShaderMacros(),
		"CSMAIN",
		"cs_5_0");

	m_csShader->setTexture2d("InputMap", srcTex2d);
	m_csShader->setTexture2d("OutputMap", uavTex);

	initQuadMesh();

	m_renderState.createState();
}

void ComputerShaderProcessTexture::initQuadMesh()
{
	m_quadMesh = MeshDX11Ptr(new MeshDX11);

	Vertex_Pos_UV0 data[4];
	data[0].pos = Vector3f(-1, 1, 0) ; 
	data[0].uv0 = Vector2f(0, 0);
	data[1].pos = Vector3f(1, 1, 0) ;
	data[1].uv0 = Vector2f(1, 0);
	data[2].pos = Vector3f(1, -1, 0) ;
	data[2].uv0 = Vector2f(1, 1);
	data[3].pos = Vector3f(-1, -1, 0) ;
	data[3].uv0 = Vector2f(0, 1);

	m_quadMesh->createVertexBuffer(data, sizeof(Vertex_Pos_UV0), 4 * sizeof(Vertex_Pos_UV0),4);

	uint16_t index[6] = { 0,1,2,2,3,0 };
	m_quadMesh->createIndexBuffer(index, sizeof(uint16_t), 6 * sizeof(uint16_t),6);
}

void ComputerShaderProcessTexture::render()
{
	//cs
	m_csShader->bin();

	//测试纹理为256x256，先硬编码
	RendererDX11::getInstance().getDeviceContex()->Dispatch(16,16,1);

	m_csShader->unBin();

	//draw tex
	//RenderStateDX11::getInstance().setRenderState();
	m_renderState.setRenderState(RendererDX11::getInstance().getDeviceContex());
	m_vsShader->bin();
	m_psShader->bin();
	float bf[4] = { 1,1,1,1 };
	//RendererDX11::getInstance().getDeviceContex()->OMSetBlendState(m_blendState, bf, 0xffffffff);
	//RendererDX11::getInstance().getDeviceContex()->IASetInputLayout(g_pInputLayout);
	m_quadMesh->draw();

	m_vsShader->unBin();
	m_psShader->unBin();
}