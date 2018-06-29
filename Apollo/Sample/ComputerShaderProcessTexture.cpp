#include "stdafx.h"
#include "ComputerShaderProcessTexture.h"
#include "TextureDX11ResourceFactory.h"
#include "RendererDX11.h"
#include "Texture2dDX11.h"
#include "VertexStruct.h"

using namespace Apollo;

ID3D11BlendState* m_blendState = nullptr;

void ComputerShaderProcessTexture::init()
{
	m_srcTextureHandle = TextureDX11ResourceFactory::getInstance().createResource("..\\bin\\Assets\\Texture\\BRDF.dds", "BRDF.dds","dds");
	Texture2dDX11* srcTex2d = (Texture2dDX11*)TextureDX11ResourceFactory::getInstance().getResource(m_srcTextureHandle);
	ID3D11Resource* dx11Resource = nullptr;
	srcTex2d->getSRV()->GetResource(&dx11Resource);
	ID3D11Texture2D* tex2dDX = (ID3D11Texture2D*)dx11Resource;
	D3D11_TEXTURE2D_DESC texDesc;
	tex2dDX->GetDesc(&texDesc);
	Texture2dConfigDX11 tex2dConfig;
	tex2dConfig.SetWidth(texDesc.Width);
	tex2dConfig.SetHeight(texDesc.Height);
	tex2dConfig.SetBindFlags(D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS);

	//创建cs写入的uav
	m_uavTextureHandle = TextureDX11ResourceFactory::getInstance().createTexture2D("CS_Texture_Process", tex2dConfig);

	Texture2dDX11* uavTex = (Texture2dDX11*)TextureDX11ResourceFactory::getInstance().getResource(m_uavTextureHandle);

	Texture2dDX11Ptr uavTexPtr(uavTex);
	Texture2dDX11Ptr srcTexPtr(srcTex2d);

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

	m_psShader->setTexture2d("ColorMap00", uavTexPtr);

	m_csShader = ShaderDX11Ptr(new ShaderDX11());
	m_csShader->loadShaderFromFile(ComputeShader,
		"../bin/Assets/Shader/InvertColorCS.hlsl",
		ShaderMacros(),
		"CSMAIN",
		"cs_5_0");

	m_csShader->setTexture2d("InputMap", srcTexPtr);
	m_csShader->setTexture2d("OutputMap", uavTexPtr);

	initQuadMesh();

	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;
	blendDesc.RenderTarget[0].BlendEnable = false;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	RendererDX11::getInstance().getDevice()->CreateBlendState(&blendDesc, &m_blendState);
}

void ComputerShaderProcessTexture::initQuadMesh()
{
	m_quadMesh = MeshDX11Ptr(new MeshDX11);

	Vertex_Pos data[4];
	data[0].pos = Vector4f(-1, 1, 0,0);
	data[1].pos = Vector4f(1, 1, 0,0);
	data[2].pos = Vector4f(1, -1, 0,0);
	data[3].pos = Vector4f(-1, -1, 0,0);

	m_quadMesh->createVertexBuffer(data, sizeof(Vertex_Pos), 4 * sizeof(Vertex_Pos));

	uint16_t index[6] = { 0,1,2,2,3,0 };
	m_quadMesh->createIndexBuffer(index, sizeof(uint16_t), 6 * sizeof(uint16_t));
}

void ComputerShaderProcessTexture::render()
{
	//cs
	m_csShader->bin();

	//测试纹理为256x256，先硬编码
	RendererDX11::getInstance().getDeviceContex()->Dispatch(16,16,1);

	m_csShader->unBin();

	//draw tex
	m_vsShader->bin();
	m_psShader->bin();
	float bf[4] = { 1,1,1,1 };
	RendererDX11::getInstance().getDeviceContex()->OMSetBlendState(m_blendState, bf, 0xffffffff);
	m_quadMesh->draw();

	m_vsShader->unBin();
	m_psShader->unBin();
}