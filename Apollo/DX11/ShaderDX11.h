#pragma once
#include "ShaderParameterDX11.h"
#include "Texture2dDX11.h"

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

		void unBind();

		//void setConstantBuffer(std::shared_ptr<ConstantBuffer> constantBuffer);
		//void setTexture(std::shared_ptr<Texture> texture);
		//void setSampler(std::shared_ptr<SamplerState> sampler);
		//void setStructuredBuffer(std::shared_ptr<StructuredBuffer> rwBuffer);

		void		setConstantBuffer(std::string name, ConstantBufferDX11Ptr buffer);

		void		setStructuredBuffer(std::string name, StructuredBufferDX11Ptr buffer);

		void		setTexture2d(std::string name, Texture2dDX11* tex2dDX11);

		ID3DBlob*		getBlob() { return m_shaderBlob.Get(); }

	protected:

		ShaderParameterPtr		getShaderParameter(std::string name);

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

		BlobComPtr						m_shaderBlob;		
		Microsoft::WRL::ComPtr<ID3D11ShaderReflection> m_pReflector;

		typedef std::map<std::string, ShaderParameterPtr > ParameterMap;
		ParameterMap m_shaderParameters;

	private:
	};

	typedef std::shared_ptr<ShaderDX11> ShaderDX11Ptr;
}
