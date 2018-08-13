#include "stdafx.h"
#include "ShaderDX11Factory.h"

using namespace Apollo;

HLSLDX11ResourcePtr ShaderDX11Factory::createShaderFromFile(ShaderType type, const std::string& fileName, const ShaderMacros& shaderMacros,
																				const std::string& entryPoint, const std::string& profile)
{
	//�ж���Դ�Ƿ��Ѿ�����
	for each (HLSLDX11ResourcePtr hlsl in m_hlslResourcePool)
	{
		if (hlsl->m_shaderType != type)
			break;

		if (hlsl->m_shaderFileName != fileName)
			break;

		if (hlsl->m_entryPoint != entryPoint)
			break;

		for (ShaderMacros::const_iterator it = shaderMacros.begin(); it != shaderMacros.end(); ++it)
		{
			if(hlsl->m_shaderMacros.find(it->first) == hlsl->m_shaderMacros.end())
				break;
		}

		//�ҵ���ͬ��Դ
		return hlsl;
	}
		

	return true;
}