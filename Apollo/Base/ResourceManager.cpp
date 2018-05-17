#include "stdafx.h"
#include "ResourceManager.h"
#include "LogManager.h"
#include "IResourceFactory.h"
#include "Resource.h"

using namespace Apollo;
using namespace std;
using namespace stdext;

void ResourceManager::registerResourceFactory(const std::string& type, IResourceFactory* creator)
{
	if (m_factorySuffixHashMap.find(type) != m_factorySuffixHashMap.end())
	{
		LogManager::getInstance().log("[registerResourceFactory] Resource Tpye:" + type + " already register!");
		return;
	}

	//一个creator可能注册多个类型比如纹理:dds,tga,exr，所以下面出现已经存在时正常的
	const int iType = creator->getResourceType();
	if (m_factoryTypeHashMap.find(iType) == m_factoryTypeHashMap.end())
	{
		m_factoryTypeHashMap[iType] = creator;
	}

	m_factorySuffixHashMap[type] = creator;
	
}

void ResourceManager::unRegisterResourceFactory(const std::string& type, IResourceFactory* creator)
{
	if (m_factorySuffixHashMap.find(type) == m_factorySuffixHashMap.end())
	{
		LogManager::getInstance().log("[unRegisterResourceFactory] Resource Tpye:" + type + " not register!");
		return;
	}

	FactorySuffixHashMap::iterator it = m_factorySuffixHashMap.find(type);
	if (it->second != creator)
	{
		LogManager::getInstance().log("[unRegisterResourceFactory] Resource Tpye:" + type + ",it->second != creator");
		return;
	}

	FactoryTypeHashMap::iterator iiit = m_factoryTypeHashMap.find(creator->getResourceType());
	if (iiit != m_factoryTypeHashMap.end())
		m_factoryTypeHashMap.erase(iiit);

	m_factorySuffixHashMap.erase(it);
}

uint32_t ResourceManager::createResource(const std::string& path, const std::string& name, const std::string& type)
{
	if (m_factorySuffixHashMap.find(type) == m_factorySuffixHashMap.end())
	{
		LogManager::getInstance().log("[ResourceManager::createResource] Resource type:" + type + " no registerResourceFactory!");
		return 0;
	}

	uint32_t handle = m_factorySuffixHashMap[type]->createResource(path, name, type);

	if (m_handleHashMap.find(name) != m_handleHashMap.end())
	{
		LogManager::getInstance().log("[ResourceManager::createResource] resource:" + path + " have other same name!");
		return 0;
	}

	ResourceChunk resChunk;
	resChunk.path = path;
	resChunk.name = name;
	resChunk.type = type;
	m_resourceChunkMap[name] = resChunk;

	m_handleHashMap[name] = handle;

	return handle;
}

uint32_t ResourceManager::getResourceHandle(const std::string& name)
{
	if (m_handleHashMap.find(name) == m_handleHashMap.end())
	{
		LogManager::getInstance().log("[ResourceManager::getResource] resource:" + name + " not exit!");
		return 0;
	}

	uint32_t handle = m_handleHashMap[name];

	if (handle == 0)
	{
		if (m_resourceChunkMap.find(name) == m_resourceChunkMap.end())
		{
			LogManager::getInstance().log("[ResourceManager::getResource] resource:" + name + " resource chunk not exit!");
			return 0;
		}

		const ResourceChunk& resChunk = m_resourceChunkMap[name];
		//可能还没加载，尝试加载
		return createResource(resChunk.path, resChunk.name, resChunk.type);
	}
	return handle;
}

IResourceFactory* ResourceManager::getResourceFactory(uint32_t handle)
{
	int resourceType = GET_RESOURCE_TYPE(handle);

	if (m_factoryTypeHashMap.find(resourceType) == m_factoryTypeHashMap.end())
	{
		stringstream ss;
		ss << "[ResourceManager::getResourceFactory] type:" + resourceType + "  not exit!";
		LogManager::getInstance().log(ss.str());
		return 0;
	}

	return m_factoryTypeHashMap[resourceType];
}

Resource* ResourceManager::getResource(uint32_t handle)
{
	IResourceFactory* creator = getResourceFactory(handle);
	return creator->getResource(handle);
}