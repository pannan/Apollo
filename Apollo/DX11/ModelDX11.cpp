#include "stdafx.h"
#include "ModelDX11.h"
#include "Graphics/SDKMesh.h"
#include "CharacterTools.h"
#include "TextureDX11ResourceFactory.h"

using namespace Apollo;


void ModelDX11::loadMaterialResources(const MaterialDX11& material)
{
	if (!material.m_albedoMap.empty())
	{
		const std::string path = "..\\bin\\Assets\\Texture\\" + m_modelName + "\\" + material.m_albedoMap;
		uint32_t texHandle = TextureDX11ResourceFactory::getInstance().createResource(path, material.m_albedoMap,"dds");
		Texture2dDX11* albedoTex = (Texture2dDX11*)TextureDX11ResourceFactory::getInstance().getResource(texHandle);

		if (material.m_ps)
		{
			material.m_ps->setTexture2d("albedoMap", albedoTex);
		}
	}

	if (!material.m_normalMap.empty())
	{
		const std::string path = "..\\bin\\Assets\\Texture\\" + m_modelName + "\\" + material.m_normalMap;
		uint32_t texHandle = TextureDX11ResourceFactory::getInstance().createResource(path, material.m_normalMap,"dds");
		Texture2dDX11* normalTex = (Texture2dDX11*)TextureDX11ResourceFactory::getInstance().getResource(texHandle);

		if (material.m_ps)
		{
			material.m_ps->setTexture2d("normalMap", normalTex);
		}
	}
}

void ModelDX11::createFromSDKMeshFile(LPCWSTR fileName)
{
	SDKMesh sdkMesh;
	std::wstring modelName = fileName;
	WStringToString(modelName, m_modelName);
	const std::wstring modelPath = L"..\\bin\\Assets\\" + modelName;
	sdkMesh.Create(modelPath.c_str());

	uint32 numMaterials = sdkMesh.GetNumMaterials();
	for (uint32 i = 0; i < numMaterials; ++i)
	{
		MaterialDX11 material;
		SDKMESH_MATERIAL* mat = sdkMesh.GetMaterial(i);

		material.m_albedoMap = mat->DiffuseTexture;// AnsiToWString(mat->DiffuseTexture);
		material.m_normalMap = mat->NormalTexture;// AnsiToWString(mat->NormalTexture);

		loadMaterialResources(material);

		m_materialList.push_back(material);
	}

	
}