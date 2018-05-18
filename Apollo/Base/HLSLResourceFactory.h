#pragma once

#include "IResourceFactory.h"
#include "Singleton.h"
#include "Resource.h"

namespace Apollo
{
	class HLSLResource;

	class HLSLResourceFactory : public IResourceFactory , public SingletonEx<HLSLResourceFactory>
	{
	public:

		HLSLResourceFactory();

		~HLSLResourceFactory();

		virtual int				getResourceType() { return RT_HLSL; }

		virtual uint32_t	createResource(const std::string& path, const std::string& name, const std::string& type);

		virtual Resource*	getResource(uint32_t handle);

	protected:

		std::vector<HLSLResource*>		m_hlslResourceList;

	private:
	};
}
