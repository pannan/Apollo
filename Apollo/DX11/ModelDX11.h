#pragma once
#include "MeshDX11.h"
#include "MaterialDX11.h"
namespace Apollo
{
	class ModelDX11
	{
	public:

		void		createFromSDKMeshFile(LPCWSTR fileName);

	protected:

		void		loadMaterialResources(const MaterialDX11& material);

	protected:

		std::string							m_modelName;

		std::vector<MeshDX11> m_meshList;

		std::vector<MaterialDX11> m_materialList;

	private:
	};
}
