#pragma once

#include "Resource.h"

namespace Apollo
{
	class HLSLResource : public Resource
	{
	public:

		HLSLResource(const std::string& path,uint32_t handle) : Resource(path, handle)
		{

		}

		~HLSLResource();

	protected:
	private:

		HLSLResource();
	};
}
