#pragma once

namespace Apollo
{
	class IResourceFactory
	{
	public:

		virtual ~IResourceFactory() = 0;

		virtual uint32_t	createResource(const std::string& path, const std::string& name, const std::string& type) = 0;

	protected:
	private:
	};
}
