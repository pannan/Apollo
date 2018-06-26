#include "stdafx.h"
#include "ComputerShaderProcessTexture.h"
#include "TextureDX11ResourceFactory.h"
#include "RendererDX11.h"

using namespace Apollo;

void ComputerShaderProcessTexture::init()
{
	m_textureHandle = TextureDX11ResourceFactory::getInstance().createResource("..\\bin\\Assets\\Texture\\BRDF.dds", "BRDF.dds");

	RendererDX11::getInstance().cre

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