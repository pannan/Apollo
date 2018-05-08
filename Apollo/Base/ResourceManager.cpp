#include "stdafx.h"
#include "ResourceManager.h"
#include "LogManager.h"
#include "IResourceFactory.h"

using namespace Apollo;
using namespace std;
using namespace stdext;

void ResourceManager::registerResourceFactory(const std::string& type, IResourceFactory* creator)
{
	if (m_factoryHashMap.find(type) != m_factoryHashMap.end())
	{
		LogManager::getInstance().log("[registerResourceFactory] Resource Tpye:" + type + " already register!");
		return;
	}

	m_factoryHashMap[type] = creator;
}

void ResourceManager::unRegisterResourceFactory(const std::string& type, IResourceFactory* creator)
{
	if (m_factoryHashMap.find(type) == m_factoryHashMap.end())
	{
		LogManager::getInstance().log("[unRegisterResourceFactory] Resource Tpye:" + type + " not register!");
		return;
	}

	FactoryHashMap::iterator it = m_factoryHashMap.find(type);
	if (it->second != creator)
	{
		LogManager::getInstance().log("[unRegisterResourceFactory] Resource Tpye:" + type + ",it->second != creator");
		return;
	}

	m_factoryHashMap.erase(it);
}

uint32_t ResourceManager::createResource(const std::string& path, const std::string& name, const std::string& type)
{
	if (m_factoryHashMap.find(type) == m_factoryHashMap.end())
	{
		LogManager::getInstance().log("[ResourceManager::createResource] Resource type:" + type + " no registerResourceFactory!");
		return 0;
	}

	uint32_t handle = m_factoryHashMap[type]->createResource(path, name, type);

	if (m_handleHashMap.find(name) != m_handleHashMap.end())
	{
		LogManager::getInstance().log("[ResourceManager::createResource] resource:" + path + " have other same name!");
		return 0;
	}

	m_handleHashMap[name] = handle;

	return handle;
}

uint32_t ResourceManager::getResource(const std::string& name)
{
	if (m_handleHashMap.find(name) == m_handleHashMap.end())
	{
		LogManager::getInstance().log("[ResourceManager::getResource] resource:" + name + " not exit!");
		return 0;
	}

	uint32_t handle = m_handleHashMap[name];

	if (handle == 0)
	{
		//可能还没加载，尝试加载
		handle = createResource()
	}
	return ;
}