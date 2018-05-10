#pragma once

#include "IResourceFactory.h"
#include "Singleton.h"

namespace Apollo
{
	class TextureDX11ResourceFactroy : public IResourceFactory, public SingletonEx<TextureDX11ResourceFactroy>
	{
	public:
	protected:
	private:
	};
}
