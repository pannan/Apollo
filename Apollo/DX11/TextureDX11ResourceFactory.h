#pragma once

#include "IResourceFactory.h"
#include "Singleton.h"

namespace Apollo
{
	class TextureResource;

	class TextureDX11ResourceFactory : public IResourceFactory, public SingletonEx<TextureDX11ResourceFactory>
	{
	public:

		TextureDX11ResourceFactory();

		~TextureDX11ResourceFactory();

		virtual uint32_t	createResource(const std::string& path, const std::string& name, const std::string& type);

	protected:

		void		loadDDS(const std::string& path);

	protected:

		std::vector<TextureResource*>		m_textureResourceList;

	private:
	};
}
