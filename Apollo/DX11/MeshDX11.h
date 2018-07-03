#pragma once

namespace Apollo
{
	class MeshDX11
	{
	public:

		MeshDX11();
		~MeshDX11();

		void			createVertexBuffer(void* data, int vertexSize, uint32_t buffSize,uint32_t vertexCount);

		void			createIndexBuffer(void* data, int vertexSize, uint32_t buffSize,uint32_t	indexCount, DXGI_FORMAT type = DXGI_FORMAT_R16_UINT);

		void			draw();

		D3D11_PRIMITIVE_TOPOLOGY getPrimitiveType() { return m_ePrimType; }
		void			setPrimitiveType(D3D11_PRIMITIVE_TOPOLOGY type) { m_ePrimType = type; }

	protected:
	private:

		DXGI_FORMAT								m_indexType;
		D3D11_PRIMITIVE_TOPOLOGY		m_ePrimType;

		uint32_t					m_vertexCount;
		uint32_t					m_indexCount;

		uint32_t					m_strideSize;
		uint32_t					m_vertexBufferOffset;
		BufferComPtr			m_vertexBufferPtr;
		BufferComPtr			m_indexBufferPtr;
	};

	typedef std::shared_ptr<MeshDX11>	MeshDX11Ptr;
}


