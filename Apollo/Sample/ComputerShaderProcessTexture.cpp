#include "stdafx.h"
#include "ComputerShaderProcessTexture.h"
#include "TextureDX11ResourceFactory.h"
#include "RendererDX11.h"
#include "Texture2dDX11.h"

using namespace Apollo;

void ComputerShaderProcessTexture::init()
{
	m_textureHandle = TextureDX11ResourceFactory::getInstance().createResource("..\\bin\\Assets\\Texture\\BRDF.dds", "BRDF.dds");
	Texture2dDX11* tex2d = (Texture2dDX11*)TextureDX11ResourceFactory::getInstance().getResource(m_textureHandle);
	ID3D11Resource* dx11Resource = nullptr;
	tex2d->getSRV()->GetResource(&dx11Resource);
	ID3D11Texture2D* tex2dDX = (ID3D11Texture2D*)dx11Resource;
	D3D11_TEXTURE2D_DESC texDesc;
	tex2dDX->GetDesc(&texDesc);
	Texture2dConfigDX11 tex2dConfig;
	tex2dConfig.SetWidth(texDesc.Width);
	tex2dConfig.SetHeight(texDesc.Height);

	//´°¿ÚcsÐ´ÈëµÄuav
	TextureDX11ResourceFactory::getInstance().createTexture2D("CS_Texture_Process", tex2dConfig);


	m_csShader = ShaderDX11Ptr(new ShaderDX11());
	m_csShader->loadShaderFromFile(VertexShader,
		"../bin/Assets/Shader/TextureVS.hlsl",
		ShaderMacros(),
		"VSMAIN",
		"vs_5_0");

	m_psShader = ShaderDX11Ptr(new ShaderDX11());
	m_psShader->loadShaderFromFile(VertexShader,
		"../bin/Assets/Shader/TexturePS.hlsl",
		ShaderMacros(),
		"PSMAIN",
		"ps_5_0");

	m_csShader = ShaderDX11Ptr(new ShaderDX11());
	m_csShader->loadShaderFromFile(VertexShader,
		"../bin/Assets/Shader/InvertColorCS.hlsl",
		ShaderMacros(),
		"CSMAIN",
		"cs_5_0");
}