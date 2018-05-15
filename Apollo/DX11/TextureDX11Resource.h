#pragma once

#include "Resource.h"

namespace Apollo
{
	class TextureDX11Resource : public Resource
	{
	public:

		TextureDX11Resource(const std::string& path, uint32_t handle) : Resource(path, handle)
		{
		}

		~TextureDX11Resource() {}

	protected:
	private:
	};
}
