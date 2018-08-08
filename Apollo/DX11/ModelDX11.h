#pragma once
#include "MeshDX11.h"
#include "MaterialDX11.h"
#include "ShaderDX11.h"
namespace Apollo
{

	class ModelDX11
	{
	public:

		void		createFromSDKMeshFile(LPCWSTR fileName, ShaderDX11Ptr vsShader);

		void			createFromMemory(void* vertexBuffer, int vertexSize, uint32_t vertexCount, void* indexBuffer, uint32_t	indexCount, DXGI_FORMAT type = DXGI_FORMAT_R16_UINT);

		void		draw();

		void		addMaterial(const MaterialPtr& mat) { m_materialList.push_back(mat); }

		uint32_t	getSubMeshCount() { return m_subMeshList.size(); }

		SubMeshDX11*		getSubMesh(uint32_t index) { return m_subMeshList[index]; }

		uint32_t	getMaterialCount() { return m_materialList.size(); }

		MaterialPtr&		getMaterial(uint32_t index) { return m_materialList[index]; }

	protected:

		void		loadMaterialResources(const MaterialDX11& material);

	protected:

		std::string							m_modelName;
		std::string							m_modelPath;

		std::vector<MeshDX11> m_meshList;

		std::vector<MaterialPtr> m_materialList;

		std::vector<SubMeshDX11*>		m_subMeshList;

	private:
	};

	typedef std::shared_ptr<ModelDX11> ModelDX11Ptr;
}
