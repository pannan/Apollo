#pragma once

#include "Singleton.h"

namespace Apollo
{
	class IResourceFactory;

	class ResourceManager : public SingletonEx<ResourceManager>
	{
	public:

		uint32_t	createResource(const std::string& path, const std::string& name, const std::string& type);

		//这里的name包含后缀
		uint32_t	getResource(const std::string& name);

		void			registerResourceFactory(const std::string& type, IResourceFactory* creator);

		void			unRegisterResourceFactory(const std::string& type, IResourceFactory* creator);

	protected:


	protected:

		typedef stdext::hash_map<std::string, IResourceFactory*>	FactoryHashMap;
		typedef stdext::hash_map<std::string, uint32_t>					ResourceHandleHashMap;

		FactoryHashMap							m_factoryHashMap;
		ResourceHandleHashMap			m_handleHashMap;

	private:
	};
}
