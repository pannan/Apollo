#pragma once

#include "Singleton.h"
#include "AssetsDirectoryViewUI.h"

namespace Apollo
{
	class UIRoot : public SingletonEx<UIRoot>
	{
	public:

		void render(int w,int h);

	protected:

		AssetsDirectoryViewUI	m_assetsDirectoryViewUI;

	private:
	};
}
