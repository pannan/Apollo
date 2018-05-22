#pragma once

#include "ISelectPropertyUI.h"
#include "Singleton.h"

namespace Apollo
{
	class TextureSelectPropertyUI : public ISelectPropertyUI,public SingletonEx<TextureSelectPropertyUI>
	{
	public:

		TextureSelectPropertyUI();
		~TextureSelectPropertyUI();

		virtual void render();

	protected:

		void			renderCubeTexture(TextureCubeMapDX11* cubeMap);
	private:
	};
}
