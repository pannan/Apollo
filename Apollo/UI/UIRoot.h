#pragma once

#include "Singleton.h"

namespace Apollo
{
	class AssetsDirectoryViewUI;
	class AssetsPropertyPanelUI;
	class TextureSelectPropertyUI;

	class UIRoot : public SingletonEx<UIRoot>
	{
	public:

		UIRoot();
		~UIRoot();

		void render(int w,int h);

	protected:

		AssetsDirectoryViewUI*	m_assetsDirectoryViewUI;
		AssetsPropertyPanelUI*	m_assetsPropertyPanelUI;
		TextureSelectPropertyUI*	m_textureSelectPropertyUI;


	private:
	};
}
