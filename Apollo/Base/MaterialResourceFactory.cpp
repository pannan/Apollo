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

	for each (ChunkData chunk in materialChunk.m_chunkList)
	{
		if (chunk.name == "vs_shader")
		{
			for each (KeyValueChunk var in chunk.elementList)
			{
				if (var.key == "source")
				{
					materialRes->m_vs.shaderFile = var.value;
					materialRes->m_vs.shaderHandle = ResourceManager::getInstance().getResourceHandle(var.value);
				}
				else if (var.key == "entry")
				{
					materialRes->m_vs.entryFunc = var.value;
				}
			}
		}
		else if (chunk.name == "ps_shader")
		{
			for each (KeyValueChunk var in chunk.elementList)
			{
				if (var.key == "source")
				{
					materialRes->m_ps.shaderFile = var.value;
					materialRes->m_ps.shaderHandle = ResourceManager::getInstance().getResourceHandle(var.value);
				}
				else if (var.key == "entry")
				{
					materialRes->m_ps.entryFunc = var.value;
				}
			}
		}
	}	

	m_materialResourceList.push_back(materialRes);

	return handle;
}

Resource* MaterialResourceFactory::getResource(uint32_t handle)
{
	return nullptr;
}