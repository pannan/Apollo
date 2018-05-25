#pragma once

#include "Resource.h"

namespace Apollo
{
	struct VS_Shader
	{
		VS_Shader() : shaderHandle(0), shaderFile(""),entryFunc(""){}
		uint32_t	shaderHandle;

		std::string		shaderFile;
		std::string		entryFunc;
	};

	struct PS_Shader
	{
		PS_Shader() : shaderHandle(0), shaderFile(""), entryFunc("") {}
		uint32_t	shaderHandle;

		std::string		shaderFile;
		std::string		entryFunc;
	};

	class MaterialResource : public Resource
	{
	public:

		MaterialResource(const std::string& path, uint32_t handle) : Resource(path, handle)
		{
		}

		~MaterialResource(){}

//	protected:

		VS_Shader			m_vs;
		PS_Shader			m_ps;

	private:

		MaterialResource();
	};
}
