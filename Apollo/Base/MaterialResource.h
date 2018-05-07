#pragma once

#include "Resource.h"

namespace Apollo
{
	class MaterialResource : public Resource
	{
	public:

		MaterialResource(const std::string& path, uint32_t handle) : Resource(path, handle)
		{

		}

		~MaterialResource();

	protected:
	private:

		MaterialResource();
	};
}
