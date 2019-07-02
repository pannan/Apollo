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

		virtual Camera* getCamera() { return nullptr; }

	protected:


	protected:

	private:
	};
}
