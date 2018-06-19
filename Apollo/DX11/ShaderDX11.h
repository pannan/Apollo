#pragma once

namespace Apollo
{
	class ShaderDX11
	{
	public:

		bool loadShaderFromString(	ShaderType type, 
													const std::string& source, 
													const std::string& fileName,
													const ShaderMacros& shaderMacros, 
													const std::string& entryPoint, 
													const std::string& profile);


		bool loadShaderFromFile(	ShaderType type, 
												const std::string& fileName, 
												const ShaderMacros& shaderMacros, 
												const std::string& entryPoint, 
												const std::string& profile);

	protected:

		std::string				getLatestProfile(ShaderType type);

	protected:

		VSComPtr			m_vertexShader;
		DSComPtr			m_domainShader;
		HSComPtr			m_hullShader;
		GSComPtr			m_geometrySHader;
		PSComPtr			m_pixelShader;
		CSComPtr			m_computeShader;


		ShaderMacros		m_shaderMacros;
		std::string				m_entryPoint;
		std::string				m_profile;
		std::wstring			m_shaderFileName;

		BlobComPtr			m_shaderBlob;

	private:
	};
}
