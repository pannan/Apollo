#pragma once
#include "ShaderParameterDX11.h"
#include "Texture2dDX11.h"
#include "HLSLDX11Resource.h"
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

		//ID3DBlob*		getBlob() { return m_shaderBlob.Get(); }

	protected:

		ShaderParameterPtr		getShaderParameter(std::string name);


		void									release();

	protected:

		typedef std::map<std::string, ShaderParameterPtr > ParameterMap;
		ParameterMap m_shaderParameters;

		HLSLDX11ResourcePtr	m_hlslResourcePtr;

	private:
	};

	typedef std::shared_ptr<ShaderDX11> ShaderDX11Ptr;
}
