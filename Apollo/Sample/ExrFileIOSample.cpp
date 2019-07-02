#include "stdafx.h"
#include "ExrFileIOSample.h"
#include "TextureDX11ResourceFactory.h"
#include "RendererDX11.h"
#include "Texture2dDX11.h"
#include "DirectXTexEXR.h"
#include "CharacterTools.h"

using namespace Apollo;
using namespace DirectX;

ExrFileIOSample::ExrFileIOSample()
{

}

ExrFileIOSample::~ExrFileIOSample()
{

}

void ExrFileIOSample::init()
{
	ScratchImage image;
	TexMetadata metadata;

	std::string exrPath = "..\\bin\\Assets\\Texture\\studio_03.exr";
	std::wstring wExrPath;
	StringToWString(exrPath, wExrPath);
	HRESULT hr = LoadFromEXRFile(wExrPath.c_str(), &metadata, image);

	Texture2dConfigDX11 tex2dConfig;
	tex2dConfig.SetWidth(image.GetImages()->width);
	tex2dConfig.SetHeight(image.GetImages()->height);
	tex2dConfig.SetBindFlags(D3D11_BIND_SHADER_RESOURCE);
	tex2dConfig.SetFormat(image.GetImages()->format);

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = image.GetImages()->pixels;
	data.SysMemPitch = image.GetImages()->rowPitch;
	data.SysMemSlicePitch = image.GetImages()->slicePitch;
	//创建cs写入的uav
	TextureDX11ResourceFactory::getInstance().createTexture2D("CS_Texture_Process", tex2dConfig,&data);
}