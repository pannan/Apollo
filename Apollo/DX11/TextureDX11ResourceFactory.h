#pragma once

#include "IResourceFactory.h"
#include "Singleton.h"
#include "Resource.h"
#include "Texture2dConfigDX11.h"
#include "Texture3dConfigDX11.h"
#include "TextureResource.h"

namespace Apollo
{
	class TextureResource;

	class TextureDX11ResourceFactory : public IResourceFactory, public SingletonEx<TextureDX11ResourceFactory>
	{
	public:

		TextureDX11ResourceFactory();

		~TextureDX11ResourceFactory();

		virtual	int			getResourceType() { return RT_TEXTURE; }

		virtual uint32_t	createResource(const std::string& path, const std::string& name, const std::string& type);

		virtual	Resource*	getResource(uint32_t handle);

		//创建一个空的纹理，没有初始化类容
		uint32_t				createTexture2D(const std::string& name, Texture2dConfigDX11& config, 
			D3D11_SUBRESOURCE_DATA* subResource = nullptr);

		uint32_t				createTexture3D(const std::string& name, Texture3dConfigDX11& config,
			D3D11_SUBRESOURCE_DATA* subResource = nullptr);

	protected:

		TextureResource*		loadDDS(const std::string& path, uint32_t handle);

		TextureResource*		loadTGA(const std::string& path, uint32_t handle);

	protected:

		std::vector<TextureResourcePtr>		m_textureResourceList;

	private:
	};
}
