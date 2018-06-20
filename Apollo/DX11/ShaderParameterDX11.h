#pragma once

namespace Apollo
{
	class ShaderParameterDX11
	{
	public:

		enum class Type
		{
			Invalid,    // Invalid parameter. Doesn't store a type.
			Texture,    // Texture.
			Sampler,    // Texture sampler.
			Buffer,     // Buffers, ConstantBuffers, StructuredBuffers.
			RWTexture,  // Texture that can be written to in a shader (using Store operations).
			RWBuffer,   // Read/write structured buffers.
		};

		ShaderParameterDX11();

		// Shader resource parameter.
		ShaderParameterDX11(const std::string& name, UINT slotID, ShaderType shaderType, Type parameterType);

		bool isValid() const;

		// Get the type of the stored parameter.
		Type getType() const;

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

		Type				m_parameterType;
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
