#include "stdafx.h"
#include "HLSLResourceFactory.h"
#include "ResourceManager.h"
#include "HLSLResource.h"

using namespace Apollo;
using namespace std;

HLSLResourceFactory::HLSLResourceFactory()
{
	ResourceManager::getInstance().registerResourceFactory("hlsl", this);
}

HLSLResourceFactory::~HLSLResourceFactory()
{
	ResourceManager::getInstance().unRegisterResourceFactory("hlsl", this);

	for each (HLSLResource* var in m_hlslResourceList)
	{
		SAFE_DELETE(var);
	}
}

uint32_t HLSLResourceFactory::createResource(const std::string& path, const std::string& name, const std::string& type)
{
	uint32_t index = m_hlslResourceList.size();
	uint32_t handle = RT_HLSL;
	handle |= (index << 8);

	HLSLResource* hlslRes = new HLSLResource(path, handle);
	m_hlslResourceList.push_back(hlslRes);

	return handle;
}

Resource* HLSLResourceFactory::getResource(uint32_t handle)
{
	return nullptr;
}