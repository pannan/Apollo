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

		void		addMaterial(const MaterialDX11& mat) { m_materialList.push_back(mat); }

	protected:

		void		loadMaterialResources(const MaterialDX11& material);

	protected:

		std::string							m_modelName;
		std::string							m_modelPath;

		std::vector<MeshDX11> m_meshList;

		std::vector<MaterialDX11> m_materialList;

	private:
	};

	typedef std::shared_ptr<ModelDX11> ModelDX11Ptr;
}
