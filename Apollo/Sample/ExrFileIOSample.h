#pragma once

#include "SampleBase.h"

namespace Apollo
{
	class ExrFileIOSample : public SampleBase
	{
	public:

		ExrFileIOSample();
		~ExrFileIOSample();

		virtual void  init();

		virtual void render(){}

		virtual void onGUI();

		virtual Camera* getCamera() { return nullptr; }

	protected:

		//加载多张单独的mipmap纹理到一个纹理
		void		combineMipmapToTexture();

	protected:

		uint32_t		m_exrTextureHandle;

	private:
	};
}
