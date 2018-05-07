#include "stdafx.h"
#include "TextureResourceFactory.h"
#include "ResourceManager.h"
#include "TextureResource.h"

using namespace Apollo;
using namespace std;

TextureResourceFactory::TextureResourceFactory()
{
	ResourceManager::getInstance().registerResourceFactory("dds", this);
	ResourceManager::getInstance().registerResourceFactory("tga", this);
	ResourceManager::getInstance().registerResourceFactory("exr", this);
}

TextureResourceFactory::~TextureResourceFactory()
{
	ResourceManager::getInstance().unregisterResourceFactory("dds", this);
	ResourceManager::getInstance().unregisterResourceFactory("tga", this);
	ResourceManager::getInstance().unregisterResourceFactory("exr", this);

	for each (TextureResource* var in m_textureResourceList)
	{
		SAFE_DELETE(var);
	}
}

uint32_t TextureResourceFactory::createResource(const std::string& path, const std::string& name, const std::string& type)
{
	uint32_t index = m_textureResourceList.size();
	uint32_t handle = RT_MATERIAL;
	handle |= (index << 8);

	TextureResource* materialRes = new TextureResource(path, handle);
	m_textureResourceList.push_back(materialRes);

	return handle;
}