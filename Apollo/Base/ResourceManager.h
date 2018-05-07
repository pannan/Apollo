#pragma once

#include "Singleton.h"

namespace Apollo
{
	class IResourceFactory;

	class ResourceManager : public SingletonEx<ResourceManager>
	{
	public:

		uint32_t	createResource(const std::string& path, const std::string& name, const std::string& type);

		void			registerResourceFactory(const std::string& type, IResourceFactory* creator);

		void			unregisterResourceFactory(const std::string& type, IResourceFactory* creator);

	protected:

		typedef stdext::hash_map<std::string, IResourceFactory*>	FactoryHashMap;
		typedef stdext::hash_map<std::string, uint32_t>					ResourceHandleHashMap;

		FactoryHashMap							m_factoryHashMap;
		ResourceHandleHashMap			m_handleHashMap;

	private:
	};
}
