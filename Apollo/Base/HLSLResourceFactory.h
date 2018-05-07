#pragma once

#include "IResourceFactory.h"
#include "Singleton.h"

namespace Apollo
{
	class HLSLResource;

	class HLSLResourceFactory : public IResourceFactory , public SingletonEx<HLSLResourceFactory>
	{
	public:

		HLSLResourceFactory();

		~HLSLResourceFactory();

		virtual uint32_t	createResource(const std::string& path, const std::string& name, const std::string& type);

	protected:

		std::vector<HLSLResource*>		m_hlslResourceList;

	private:
	};
}
