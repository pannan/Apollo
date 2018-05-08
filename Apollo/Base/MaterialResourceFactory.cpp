#include "stdafx.h"
#include "MaterialResourceFactory.h"
#include "ResourceManager.h"
#include "MaterialResource.h"
#include "MaterialParse.h"

using namespace Apollo;
using namespace std;

MaterialResourceFactory::MaterialResourceFactory()
{
	ResourceManager::getInstance().registerResourceFactory("material", this);
}

MaterialResourceFactory::~MaterialResourceFactory()
{
	ResourceManager::getInstance().unRegisterResourceFactory("material", this);

	for each (MaterialResource* var in m_materialResourceList)
	{
		SAFE_DELETE(var);
	}
}

uint32_t MaterialResourceFactory::createResource(const std::string& path, const std::string& name, const std::string& type)
{
	uint32_t index = m_materialResourceList.size();
	uint32_t handle = RT_MATERIAL;
	handle |= (index << 8);

	MaterialResource* materialRes = new MaterialResource(path, handle);

	const MaterialChunk& materialChunk = MaterialParse::getInstance().getLastParseMaterial();

	m_materialResourceList.push_back(materialRes);

	return handle;
}