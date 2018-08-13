#pragma once

#include "HLSLResource.h"

namespace Apollo
{
	class HLSLDX11Resource : public HLSLResource
	{

		friend class ShaderDX11Factory;
	public:

		HLSLDX11Resource(ShaderType type,
										const std::string& fileName,
										const ShaderMacros& shaderMacros,
										const std::string& entryPoint,
										const std::string& profile);

		virtual ~HLSLDX11Resource();

	protected:

		void release();

		bool loadShaderFromFile(ShaderType shaderType,
			const std::string& fileName,
			const ShaderMacros& shaderMacros,
			const std::string& entryPoint,
			const std::string& profile);

		bool loadShaderFromString(ShaderType type,
			const std::string& source,
			const std::string& fileName,
			const ShaderMacros& shaderMacros,
			const std::string& entryPoint,
			const std::string& profile);

		std::string				getLatestProfile(ShaderType type);
	private:

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

		BlobComPtr						m_shaderBlob;
		D3D11_SHADER_DESC	m_shaderDescription;
		Microsoft::WRL::ComPtr<ID3D11ShaderReflection> m_pReflector;
	};

	typedef std::shared_ptr<HLSLDX11Resource> HLSLDX11ResourcePtr;
}
