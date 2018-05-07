#pragma once

#include "IResourceFactory.h"
#include "Singleton.h"

namespace Apollo
{
	class MaterialResource;

	class MaterialResourceFactory : public IResourceFactory, public SingletonEx<MaterialResourceFactory>
	{
	public:

		MaterialResourceFactory();

		~MaterialResourceFactory();

		virtual uint32_t	createResource(const std::string& path, const std::string& name, const std::string& type);

	protected:

		std::vector<MaterialResource*>		m_materialResourceList;

	private:
	};
}
