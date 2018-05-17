#pragma once

namespace Apollo
{
	class Resource;

	class IResourceFactory
	{
	public:

		virtual ~IResourceFactory() {}

		virtual uint32_t	createResource(const std::string& path, const std::string& name, const std::string& type) = 0;

		virtual Resource*	getResource(uint32_t handle) = 0;

		virtual	int			getResourceType() = 0;

	protected:
	private:
	};
}
