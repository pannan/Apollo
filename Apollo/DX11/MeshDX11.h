#pragma once

namespace Apollo
{
	class MeshDX11
	{
	public:

		MeshDX11();
		~MeshDX11();

		void			createVertexBuffer(void* data, int vertexSize, uint32_t buffSize);

		void			createIndexBuffer(void* data, int vertexSize, uint32_t buffSize);

		void			draw();

		D3D11_PRIMITIVE_TOPOLOGY getPrimitiveType() { return m_ePrimType; }
		void			setPrimitiveType(D3D11_PRIMITIVE_TOPOLOGY type) { m_ePrimType = type; }

	protected:
	private:

		D3D11_PRIMITIVE_TOPOLOGY		m_ePrimType;

		uint32_t					m_strideSize;
		uint32_t					m_vertexBufferOffset;
		BufferComPtr			m_vertexBufferPtr;
		BufferComPtr			m_indexBufferPtr;
	};

	typedef std::shared_ptr<MeshDX11>	MeshDX11Ptr;
}


