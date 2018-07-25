#include "stdafx.h"
#include "ModelDX11.h"
#include "Graphics/SDKMesh.h"
#include "CharacterTools.h"
#include "TextureDX11ResourceFactory.h"
#include "MaterialDX11.h"
using namespace Apollo;


void ModelDX11::loadMaterialResources(const MaterialDX11& material)
{
	if (!material.m_albedoMap.empty())
	{
		const std::string path = m_modelPath + material.m_albedoMap;
		uint32_t texHandle = TextureDX11ResourceFactory::getInstance().createResource(path, material.m_albedoMap,"dds");
		Texture2dDX11* albedoTex = (Texture2dDX11*)TextureDX11ResourceFactory::getInstance().getResource(texHandle);

		if (material.m_ps && albedoTex)
		{
			material.m_ps->setTexture2d("DiffuseMap", albedoTex);
		}
	}

	if (!material.m_normalMap.empty())
	{
		const std::string path = m_modelPath + material.m_normalMap;
		uint32_t texHandle = TextureDX11ResourceFactory::getInstance().createResource(path, material.m_normalMap,"dds");
		Texture2dDX11* normalTex = (Texture2dDX11*)TextureDX11ResourceFactory::getInstance().getResource(texHandle);

		if (material.m_ps && normalTex)
		{
			material.m_ps->setTexture2d("normalMap", normalTex);
		}
	}
}

void ModelDX11::createFromSDKMeshFile(LPCWSTR fileName, ShaderDX11Ptr vsShader)
{
	SDKMesh sdkMesh;
	std::wstring modelName = fileName;
	std::wstring modelPath = L"..\\bin\\Assets\\Models\\" + modelName + L"\\";
	WStringToString(modelName, m_modelName);
	WStringToString(modelPath, m_modelPath);
	std::wstring modelMeshPath = modelPath + fileName + L".sdkmesh";
	sdkMesh.Create(modelMeshPath.c_str());

	uint32 numMaterials = sdkMesh.GetNumMaterials();
	for (uint32 i = 0; i < numMaterials; ++i)
	{
		MaterialDX11 material;
		SDKMESH_MATERIAL* mat = sdkMesh.GetMaterial(i);

		material.m_albedoMap = mat->DiffuseTexture;// AnsiToWString(mat->DiffuseTexture);
		material.m_normalMap = mat->NormalTexture;// AnsiToWString(mat->NormalTexture);

		//这里的vs可以同一个，不用每个都创建
		material.m_vs = vsShader;

		material.m_ps = ShaderDX11Ptr(new ShaderDX11());
		material.m_ps->loadShaderFromFile(PixelShader,
			"../bin/Assets/Shader/Mesh.hlsl",
			ShaderMacros(),
			"PSMAIN",
			"ps_5_0");

		loadMaterialResources(material);

		m_materialList.push_back(material);
	}

	uint32 numMeshes = sdkMesh.GetNumMeshes();
	m_meshList.resize(numMeshes);
	for (uint32 meshIdx = 0; meshIdx < numMeshes; ++meshIdx)
		m_meshList[meshIdx].createFromSDKMeshFile(sdkMesh,meshIdx);// Initialize(device, sdkMesh, meshIdx, generateTangentFrame);
}

void ModelDX11::createFromMemory(void* vertexBuffer, int vertexSize, uint32_t vertexCount, void* indexBuffer,
	uint32_t	indexCount, DXGI_FORMAT type)
{
	MeshDX11 mesh;
	mesh.createFromMemory(vertexBuffer, vertexSize, vertexCount, indexBuffer, indexCount, type);
	m_meshList.push_back(mesh);
}

void ModelDX11::draw()
{
	for (size_t meshID = 0; meshID < m_meshList.size(); ++meshID)
	{
		const MeshDX11& mesh = m_meshList[meshID];

		for (size_t subMeshID = 0; subMeshID < mesh.m_subMeshList.size(); ++subMeshID)
		{
			const SubMeshDX11& subMesh = mesh.m_subMeshList[subMeshID];
			const MaterialDX11& material = m_materialList[subMesh.m_materialID];

			material.bind();

			mesh.drawSubMesh(subMeshID);

			material.unBind();

		}
	}
}