#pragma once

#include "Singleton.h"

namespace Apollo
{
	class Resource;
	class IResourceFactory;

	struct ResourceChunk 
	{
		std::string		path;
		std::string		name;
		std::string		type;
	};

	class ResourceManager : public SingletonEx<ResourceManager>
	{
	public:

		uint32_t	createResource(const std::string& path, const std::string& name, const std::string& type);

		//这里的name包含后缀
		uint32_t	getResourceHandle(const std::string& name);

		Resource*	getResource(uint32_t handle);

		void			registerResourceFactory(const std::string& type, IResourceFactory* creator);

		void			unRegisterResourceFactory(const std::string& type, IResourceFactory* creator);

	protected:

		IResourceFactory*		getResourceFactory(uint32_t handle);

	protected:

		typedef stdext::hash_map<std::string, IResourceFactory*>	FactorySuffixHashMap;
		typedef stdext::hash_map<int, IResourceFactory*>				FactoryTypeHashMap;
		typedef stdext::hash_map<std::string, uint32_t>					ResourceHandleHashMap;
		typedef stdext::hash_map<std::string, ResourceChunk>		ResourceChunkMap;

		FactorySuffixHashMap				m_factorySuffixHashMap;
		FactoryTypeHashMap					m_factoryTypeHashMap;
		ResourceHandleHashMap			m_handleHashMap;
		ResourceChunkMap					m_resourceChunkMap;

	private:
	};
}
