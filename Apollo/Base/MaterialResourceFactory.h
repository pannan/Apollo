#pragma once

#include "IResourceFactory.h"
#include "Singleton.h"
#include "Resource.h"

namespace Apollo
{
	class MaterialResource;

	class MaterialResourceFactory : public IResourceFactory, public SingletonEx<MaterialResourceFactory>
	{
	public:

		MaterialResourceFactory();

		~MaterialResourceFactory();

		virtual int				getResourceType() { return RT_MATERIAL; }

		virtual uint32_t	createResource(const std::string& path, const std::string& name, const std::string& type);

		virtual Resource*	getResource(uint32_t handle);

	protected:

		std::vector<MaterialResource*>		m_materialResourceList;

	private:
	};
}
