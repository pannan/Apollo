#pragma once

#include "Singleton.h"
#include "ShaderDX11.h"
#include "HLSLDX11Resource.h"
namespace Apollo
{
	class ShaderDX11Factory : public SingletonEx<ShaderDX11Factory>
	{
	public:

		HLSLDX11ResourcePtr createShaderFromFile(	ShaderType type,
																					const std::string& fileName,
																					const ShaderMacros& shaderMacros,
																					const std::string& entryPoint,
																					const std::string& profile);

	protected:

		std::vector<HLSLDX11ResourcePtr>		m_hlslResourcePool;
	private:
	};
}
