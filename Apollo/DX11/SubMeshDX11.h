#pragma once
#include "MaterialDX11.h"
namespace Apollo
{

	class SDKMesh;

	class SubMeshDX11
	{
	public:

		friend class MeshDX11;
		friend class ModelDX11;
	protected:

		uint32_t					m_indexStart;
		uint32_t					m_vertexStart;
		uint32_t					m_vertexCount;
		uint32_t					m_indexCount;


		uint32_t					m_materialID;
	private:
	};
}
