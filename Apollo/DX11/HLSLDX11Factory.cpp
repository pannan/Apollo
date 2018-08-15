#include "stdafx.h"
#include "HLSLDX11Factory.h"

using namespace Apollo;

HLSLDX11ResourcePtr HLSLDX11Factory::createShaderFromFile(ShaderType type, const std::string& fileName, const ShaderMacros& shaderMacros,
																				const std::string& entryPoint, const std::string& profile)
{
	//判断资源是否已经加载
	for each (HLSLDX11ResourcePtr hlsl in m_hlslResourcePool)
	{
		if (hlsl->m_shaderType != type)
			continue;

		if (hlsl->m_shaderFileName != fileName)
			continue;

		if (hlsl->m_entryPoint != entryPoint)
			continue;

		for (ShaderMacros::const_iterator it = shaderMacros.begin(); it != shaderMacros.end(); ++it)
		{
			if(hlsl->m_shaderMacros.find(it->first) == hlsl->m_shaderMacros.end())
				continue;
		}

		//找到相同资源
		return hlsl;
	}

	HLSLDX11ResourcePtr hlslPtr = HLSLDX11ResourcePtr(new HLSLDX11Resource(type, fileName, shaderMacros, entryPoint, profile));
	m_hlslResourcePool.push_back(hlslPtr);

	return hlslPtr;
}