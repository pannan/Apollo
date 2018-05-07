#pragma once

#include "Resource.h"

namespace Apollo
{
	class TextureResource : public Resource
	{
	public:

		TextureResource(const std::string& path, uint32_t handle) : Resource(path, handle)
		{

		}

		~TextureResource();

	protected:
	private:

		TextureResource();
	};
}