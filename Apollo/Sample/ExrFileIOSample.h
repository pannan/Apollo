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


	protected:

		uint32_t		m_exrTextureHandle;

	private:
	};
}
