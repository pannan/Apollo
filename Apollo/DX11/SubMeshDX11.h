#pragma once

#include "Graphics/IRenderable.h"

namespace Apollo
{

	class SDKMesh;
	class MeshDX11;

	class SubMeshDX11 : public IRenderable
	{
	public:

		SubMeshDX11(): m_materialID(0),m_parent(nullptr), m_subMeshID(0){}

		friend class MeshDX11;
		friend class ModelDX11;

		void			setMaterialID(uint16_t id);

		uint16_t	getMaterialID() { return m_materialID; }

		virtual void	render();

	protected:

		uint32_t					m_indexStart;
		uint32_t					m_vertexStart;
		uint32_t					m_vertexCount;
		uint32_t					m_indexCount;

		uint16_t					m_subMeshID;
		uint16_t					m_materialID;
		MeshDX11*				m_parent;
	private:
	};
}
