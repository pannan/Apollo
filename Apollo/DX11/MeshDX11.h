#pragma once

using namespace Apollo;

class MeshDX11
{
public:

	MeshDX11();
	~MeshDX11();

	void			createVertexBuffer(void* data,int vertexSize,uint32_t buffSize);

	void			createIndexBuffer(void* data, int vertexSize, uint32_t buffSize);

	void			bind();

	void			draw();

protected:
private:

	D3D11_PRIMITIVE_TOPOLOGY		m_ePrimType;

	uint32_t					m_strideSize;
	uint32_t					m_vertexBufferOffset;
	BufferComPtr			m_vertexBufferPtr;
	BufferComPtr			m_indexBufferPtr;
};
