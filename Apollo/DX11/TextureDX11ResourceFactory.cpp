#include "stdafx.h"
#include "TextureDX11ResourceFactory.h"
#include "ResourceManager.h"
#include "TextureResource.h"
#include "LogManager.h"
#include "DirectXTex.h"
#include "CharacterTools.h"
#include "DX11Renderer.h"

using namespace Apollo;
using namespace std;
using namespace DirectX;

TextureDX11ResourceFactory::TextureDX11ResourceFactory()
{
	ResourceManager::getInstance().registerResourceFactory("dds", this);
	ResourceManager::getInstance().registerResourceFactory("tga", this);
	ResourceManager::getInstance().registerResourceFactory("exr", this);
}

TextureDX11ResourceFactory::~TextureDX11ResourceFactory()
{
	ResourceManager::getInstance().unRegisterResourceFactory("dds", this);
	ResourceManager::getInstance().unRegisterResourceFactory("tga", this);
	ResourceManager::getInstance().unRegisterResourceFactory("exr", this);

	for each (TextureResource* var in m_textureResourceList)
	{
		SAFE_DELETE(var);
	}
}

void TextureDX11ResourceFactory::loadDDS(const std::string& path)
{
	TexMetadata mdata;

	wstring wpath;
	StringToWString(path, wpath);
	HRESULT hr = GetMetadataFromDDSFile(wpath.c_str(), DDS_FLAGS_NONE, mdata);
	if (hr != S_OK)
	{
		LogManager::getInstance().log("[TextureDX11ResourceFactory::loadDDS] GetMetadataFromDDSFile error! file:" + path);
		return;
	}

	ScratchImage image;
	hr = LoadFromDDSFile(wpath.c_str(), DDS_FLAGS_NONE, &mdata, image);
	if (FAILED(hr))
	{
		LogManager::getInstance().log("[TextureDX11ResourceFactory::loadDDS] LoadFromDDSFile error! file:" + path);
		return;
	}

	// Special case to make sure Texture cubes remain arrays
	//mdata.miscFlags &= ~TEX_MISC_TEXTURECUBE;
	ID3D11ShaderResourceView* srv = nullptr;
	hr = CreateShaderResourceView(	DX11Renderer::getInstance().getDevice(), 
															image.GetImages(), 
															image.GetImageCount(), 
															mdata, 
															&srv);

	if (hr != S_OK)
	{
		LogManager::getInstance().log("[TextureDX11ResourceFactory::loadDDS] LoadFromDDSFile error! file:" + path);
		return;
	}
	
}

uint32_t TextureDX11ResourceFactory::createResource(const std::string& path, const std::string& name, const std::string& type)
{
	uint32_t index = m_textureResourceList.size();
	uint32_t handle = RT_MATERIAL;
	handle |= (index << 8);

	//ÅÐ¶Ïºó×ºÃû
	int pos = name.find_last_not_of('.');
	if (pos == std::string::npos)
	{
		LogManager::getInstance().log("[TextureDX11ResourceFactory::createResource] name error!");
		return 0;
	}
	
	string suffixName = name.substr(pos + 1, name.size() - pos);

	if (suffixName == "dds")
	{

	}

	//TextureResource* textureRes = new TextureResource(path, handle);
	//m_textureResourceList.push_back(textureRes);

	return handle;
}