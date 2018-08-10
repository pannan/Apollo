#pragma once

#include "Singleton.h"
#include "ShaderDX11.h"
namespace Apollo
{
	class ShaderDX11Factory : public SingletonEx<ShaderDX11Factory>
	{
	public:

	bool createShaderFromFile(ShaderType type,const std::string& fileName,const ShaderMacros& shaderMacros,const std::string& entryPoint,const std::string& profile);

	protected:

		std::vector<ShaderDX11Ptr>		m_shaderList;
	private:
	};
}
