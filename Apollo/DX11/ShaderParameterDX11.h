#pragma once

#include "ConstantBufferDX11.h"
#include "StructuredBufferDX11.h"
#include "Texture2dDX11.h"

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

//	protected:

		void setConstantBuffer(ConstantBufferDX11Ptr buffer) { m_constantBufferPtr = buffer; }
	//	void setTexture(std::shared_ptr<Texture> texture);
	//	void setSampler(std::shared_ptr<SamplerState> sampler);
		void setStructuredBuffer(StructuredBufferDX11Ptr buffer) { m_structuredBufferPtr = buffer; }

		void	setTexture2d(Texture2dDX11* tex2d) { m_texture2dDX11 = tex2d; }


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
		//ShaderResourceViewComPtr		m_srvPtr;
		//UnorderedAccessViewComPtr	m_uavPtr;

		ConstantBufferDX11Ptr				m_constantBufferPtr;
		StructuredBufferDX11Ptr			m_structuredBufferPtr;

		Texture2dDX11*							m_texture2dDX11;


	private:
	};

	typedef std::shared_ptr<ShaderParameterDX11>	ShaderParameterPtr;
}
