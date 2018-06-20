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

		void bin();

		void unBin();

	protected:

		std::string				getLatestProfile(ShaderType type);

		void						release();

	protected:

		ShaderType						m_shaderType;

		VSComPtr						m_vertexShader;
		DSComPtr						m_domainShader;
		HSComPtr						m_hullShader;
		GSComPtr						m_geometrySHader;
		PSComPtr						m_pixelShader;
		CSComPtr						m_computeShader;
		InputLayoutComPtr			m_inputLayoutPtr;


		ShaderMacros					m_shaderMacros;
		std::string							m_entryPoint;
		std::string							m_profile;
		std::string							m_shaderFileName;

		BlobComPtr			m_shaderBlob;

	private:
	};

	typedef std::shared_ptr<ShaderDX11> ShaderDX11Ptr;
}
