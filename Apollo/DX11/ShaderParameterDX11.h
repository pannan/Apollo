#pragma once

namespace Apollo
{
	/*
	constantbuffer:资源是位于显存的地址
	调用ID3D11DeviceContext1::Map来从主内存把数据传输到显存
	调用ID3D11DeviceContext1::xxSetConstantBuffers来把buffer在显存地址设置到指定寄存器，这样GPU可以访问这个地址的数据

	structbuffer:
	调用ID3D11DeviceContext1::Map来从主内存把数据传输到显存
	调用ID3D11DeviceContext1::xxSetShaderResources或者
												  xxSetUnorderedAccessViews来把buffer在显存地址设置到指定寄存器，这样GPU可以访问这个地址的数据
	*/
	class ShaderParameterDX11
	{
	public:

		ShaderParameterDX11();

		// Shader resource parameter.
		ShaderParameterDX11(const std::string& name, UINT slotID, ShaderType shaderType, ShaderParameterType parameterType);

		bool isValid() const;

		// Get the type of the stored parameter.
		ShaderParameterType getType() const;

		// Bind the shader parameter to a specific slot for the given shader type.
		void bind();
		void unBind();

	protected:

		/*void setConstantBuffer(std::shared_ptr<ConstantBuffer> buffer);
		void setTexture(std::shared_ptr<Texture> texture);
		void setSampler(std::shared_ptr<SamplerState> sampler);
		void setStructuredBuffer(std::shared_ptr<StructuredBuffer> rwBuffer);*/


	protected:

		std::string m_Name;

		UINT			m_slotID;

		ShaderType	m_shaderType;

		ShaderParameterType				m_parameterType;
		// Shader parameter does not take ownership of these types.
		/*std::weak_ptr<Texture> m_pTexture;
		std::weak_ptr<SamplerState> m_pSamplerState;
		std::weak_ptr<ConstantBuffer> m_pConstantBuffer;
		std::weak_ptr<StructuredBuffer> m_pStructuredBuffer;*/
		ShaderResourceViewComPtr		m_srvPtr;
		UnorderedAccessViewComPtr	m_uavPtr;


	private:
	};
}
