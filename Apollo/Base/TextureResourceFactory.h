#pragma once

#include "IResourceFactory.h"
#include "Singleton.h"

namespace Apollo
{
	class TextureResource;

	class TextureResourceFactory : public IResourceFactory, public SingletonEx<TextureResourceFactory>
	{
	public:

		TextureResourceFactory();

		~TextureResourceFactory();

		virtual uint32_t	createResource(const std::string& path, const std::string& name, const std::string& type);

	protected:

		std::vector<TextureResource*>		m_textureResourceList;

	private:
	};
}
