#pragma once

namespace Apollo
{
	/*
	constantbuffer:��Դ��λ���Դ�ĵ�ַ
	����ID3D11DeviceContext1::Map�������ڴ�����ݴ��䵽�Դ�
	����ID3D11DeviceContext1::xxSetConstantBuffers����buffer���Դ��ַ���õ�ָ���Ĵ���������GPU���Է��������ַ������

	structbuffer:
	����ID3D11DeviceContext1::Map�������ڴ�����ݴ��䵽�Դ�
	����ID3D11DeviceContext1::xxSetShaderResources����
												  xxSetUnorderedAccessViews����buffer���Դ��ַ���õ�ָ���Ĵ���������GPU���Է��������ַ������
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
