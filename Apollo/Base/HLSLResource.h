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

		virtual ~HLSLResource(){}

		/*	virtual void bind() = 0;

			virtual void unBind() = 0;*/

	protected:

		ShaderType						m_shaderType;

	private:

		HLSLResource();
	};
}
