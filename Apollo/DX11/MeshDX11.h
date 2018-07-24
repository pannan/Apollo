#pragma once
#include "MaterialDX11.h"
#include "SubMeshDX11.h"
namespace Apollo
{
	class MeshDX11
	{
	public:

		MeshDX11();
		~MeshDX11();

		void			createFromSDKMeshFile(SDKMesh& sdkMesh, uint32 meshIdx);

		void			createFromMemory(void* vertexBuffer, int vertexSize, uint32_t vertexCount,void* indexBuffer,uint32_t	indexCount, DXGI_FORMAT type = DXGI_FORMAT_R16_UINT);

		void			createVertexBuffer(void* data, int vertexSize, uint32_t buffSize,uint32_t vertexCount);

		void			createIndexBuffer(void* data, int vertexSize, uint32_t buffSize,uint32_t	indexCount, DXGI_FORMAT type = DXGI_FORMAT_R16_UINT);

		void			draw();

		D3D11_PRIMITIVE_TOPOLOGY getPrimitiveType() { return m_ePrimType; }
		void			setPrimitiveType(D3D11_PRIMITIVE_TOPOLOGY type) { m_ePrimType = type; }

	protected:

		void		loadMaterialResources(const MaterialDX11& material);

	private:

		std::string											m_modelName;

		DXGI_FORMAT								m_indexType;
		D3D11_PRIMITIVE_TOPOLOGY		m_ePrimType;

		uint32_t					m_vertexCount;
		uint32_t					m_indexCount;

		uint32_t					m_strideSize;
		uint32_t					m_vertexBufferOffset;
		BufferComPtr			m_vertexBufferPtr;
		BufferComPtr			m_indexBufferPtr;

		std::vector<SubMeshDX11>	m_subMeshList;
	};

	typedef std::shared_ptr<MeshDX11>	MeshDX11Ptr;
}


