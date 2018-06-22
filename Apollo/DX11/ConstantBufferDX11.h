#pragma once

namespace Apollo
{
	class ConstantBufferDX11
	{
	public:
		ConstantBufferDX11(size_t size);

		ConstantBufferDX11(size_t size,bool dynamic, bool CPUupdates, D3D11_SUBRESOURCE_DATA* pData);

		virtual ~ConstantBufferDX11();

		bool bind(unsigned int id, ShaderType shaderType, ShaderParameterType parameterType);
		void unBind(unsigned int id, ShaderType shaderType, ShaderParameterType parameterType);

		//void copy(std::shared_ptr<ConstantBuffer> other);
		void set(const void* data, size_t size);

	protected:
		//virtual void Copy(std::shared_ptr<Buffer> other);
		//

	private:
	
		size_t  m_bufferSize;

		BufferComPtr m_bufferComPtr;
	};
}
