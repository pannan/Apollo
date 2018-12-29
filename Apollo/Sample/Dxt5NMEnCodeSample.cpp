#include "stdafx.h"
#include "Dxt5NMEnCodeSample.h"
#include "TextureDX11ResourceFactory.h"
#include "RendererDX11.h"
#include "Texture2dDX11.h"
#include "SDK\\TGAImage.h"

using namespace Apollo;

Dxt5NMEnCodeSample::Dxt5NMEnCodeSample()
{

}

Dxt5NMEnCodeSample::~Dxt5NMEnCodeSample()
{

}

void Dxt5NMEnCodeSample::init()
{
	//uint32_t texHandle = TextureDX11ResourceFactory::getInstance().createResource("..\\bin\\Assets\\Texture\\2_wj_08_n.tga", "2_wj_08_n.tga", "tga");
	//Texture2dDX11* srcTex2d = (Texture2dDX11*)TextureDX11ResourceFactory::getInstance().getResource(texHandle);
	//ID3D11Texture2D* tex2dDX = srcTex2d->getTexture2D();// ID3D11Texture2D*)dx11Resource;
	//D3D11_TEXTURE2D_DESC texDesc;
	//tex2dDX->GetDesc(&texDesc);
	//Texture2dConfigDX11 tex2dConfig;
	//tex2dConfig.SetWidth(texDesc.Width);
	//tex2dConfig.SetHeight(texDesc.Height);
	//tex2dConfig.SetBindFlags(D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS);

	TGAImage tgaImage;
	TGACODE tgaCode = tgaImage.Load("..\\bin\\Assets\\Texture\\2_wj_08_n.tga");


	TGAStruct* srcTgaData = tgaImage.getTGAData();

	TGAImage desImage;
	TGAStruct* desTgaData = desImage.getTGAData();
	memcpy(&desTgaData->header, &srcTgaData->header, sizeof(TGAHeader));
	desTgaData->header.pixelDepth = 32;
	desTgaData->header.imageDescriptor = 8;

	desTgaData->data.colorMapData = nullptr;
	desTgaData->data.imageId = nullptr;
	desTgaData->data.imageData = new char[desTgaData->header.width * desTgaData->header.height * 4];

	for (int y = 0; y < desTgaData->header.height; ++y)
	{
		for (int x = 0; x < desTgaData->header.width; ++x)
		{
			uint32_t index = y * desTgaData->header.width * 3 + x * 3;
			char r = srcTgaData->data.imageData[index];
			char g = srcTgaData->data.imageData[index + 1];
			char b = srcTgaData->data.imageData[index + 2];

			uint32_t desIndex = y * desTgaData->header.width * 4 + x * 4;

			//to dxt5nm encode
			//x ±£´æ a
			//y±£´æg
			//z¶ªÆú
			desTgaData->data.imageData[desIndex] = 0;
			desTgaData->data.imageData[desIndex + 1] = g;
			desTgaData->data.imageData[desIndex + 2] = 0;
			desTgaData->data.imageData[desIndex + 3] = r;
		}
	}

	desImage.Save("..\\bin\\Assets\\Texture\\test.tga");
	

}