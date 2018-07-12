#pragma once

namespace Apollo
{
	class StructuredBufferDX11
	{
	public:

		friend class ShaderParameterDX11;

		StructuredBufferDX11(UINT count, UINT structsize, bool CPUWritable, bool GPUWritable, D3D11_SUBRESOURCE_DATA* pData);
		~StructuredBufferDX11();

		// Bind the buffer for rendering.
		bool bind(unsigned int id, ShaderType shaderType, ShaderParameterType parameterType);
		// Unbind the buffer for rendering.
		void unBind(unsigned int id, ShaderType shaderType, ShaderParameterType parameterType);

		void	commit(void* data,uint32_t size);

		// Is this an index buffer or an attribute/vertex buffer?
		//virtual BufferType GetType() const;

		// How many elements does this buffer contain?
		//virtual unsigned int GetElementCount() const;

		//virtual void Copy(std::shared_ptr<StructuredBuffer> other);

		// Clear the contents of the buffer.
		//virtual void Clear();

		// Used by the RenderTargetDX11 only.
		//ID3D11UnorderedAccessView* GetUnorderedAccessView() const;

	//protected:
	//	virtual void Copy(std::shared_ptr<Buffer> other);
	//	virtual void SetData(void* data, size_t elementSize, size_t offset, size_t numElements);
	//	// Commit the data from system memory to device memory.
	//	void Commit();

	private:

		bool													m_bUAV;
		BufferComPtr									m_bufferComPtr;
		//ShaderResourceViewComPtr				m_srcComPtr;
		//UnorderedAccessViewComPtr			m_uavComPtr;

		UINT													m_elementCount;
		UINT													m_elementSize;

		bool													m_CPUWritable;		
	};

	typedef std::shared_ptr<StructuredBufferDX11>	StructuredBufferDX11Ptr;
}
