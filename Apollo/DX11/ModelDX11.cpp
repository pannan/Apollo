#include "stdafx.h"
#include "ModelDX11.h"
#include "Graphics/SDKMesh.h"
#include "CharacterTools.h"
#include "TextureDX11ResourceFactory.h"
#include "MaterialDX11.h"
#include "VertexStruct.h"

using namespace Apollo;

//std::shared_ptr<ModelDX11>		ModelDX11::m_fullScreenModelPtr;

struct DepthSortAscendingLess
{
	bool operator()(const IRenderable* a, const IRenderable* b) const
	{
		return a->m_handle < b->m_handle;
	}
};

void ModelDX11::loadMaterialResources(MaterialDX11& material)
{
	ShaderDX11Ptr& forwardShader = material.m_ps[(uint8_t)RenderPipelineType::ForwardRender];
	ShaderDX11Ptr& deferredShader = material.m_ps[(uint8_t)RenderPipelineType::DeferredRender];
	if (!material.m_albedoMap.empty())
	{
		const std::string path = m_modelPath + material.m_albedoMap;
		uint32_t texHandle = TextureDX11ResourceFactory::getInstance().createResource(path, material.m_albedoMap,"dds");
		Texture2dDX11* albedoTex = (Texture2dDX11*)TextureDX11ResourceFactory::getInstance().getResource(texHandle);

		if (forwardShader && albedoTex)
			forwardShader->setTexture2d("DiffuseMap", albedoTex);

		if (deferredShader && albedoTex)
			deferredShader->setTexture2d("DiffuseMap", albedoTex);
	}

	if (!material.m_normalMap.empty())
	{
		const std::string path = m_modelPath + material.m_normalMap;
		uint32_t texHandle = TextureDX11ResourceFactory::getInstance().createResource(path, material.m_normalMap,"dds");
		Texture2dDX11* normalTex = (Texture2dDX11*)TextureDX11ResourceFactory::getInstance().getResource(texHandle);

		if (forwardShader && normalTex)
			forwardShader->setTexture2d("normalMap", normalTex);

		if (deferredShader && normalTex)
			deferredShader->setTexture2d("normalMap", normalTex);
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
		MaterialDX11* material = new MaterialDX11;
		SDKMESH_MATERIAL* meshMat = sdkMesh.GetMaterial(i);

		material->m_albedoMap = meshMat->DiffuseTexture;// AnsiToWString(mat->DiffuseTexture);
		material->m_normalMap = meshMat->NormalTexture;// AnsiToWString(mat->NormalTexture);

		//这里的vs可以同一个，不用每个都创建
		material->m_vs = vsShader;

		material->m_ps[(uint8_t)RenderPipelineType::ForwardRender] = ShaderDX11Ptr(new ShaderDX11());
		material->m_ps[(uint8_t)RenderPipelineType::ForwardRender]->loadShaderFromFile(ShaderType::PixelShader,
			"../bin/Assets/Shader/Mesh.hlsl",
			ShaderMacros(),
			"PSMAIN",
			"ps_5_0");

		material->m_ps[(uint8_t)RenderPipelineType::DeferredRender] = ShaderDX11Ptr(new ShaderDX11());
		material->m_ps[(uint8_t)RenderPipelineType::DeferredRender]->loadShaderFromFile(ShaderType::PixelShader,
			"../bin/Assets/Shader/Deferred.hlsl",
			ShaderMacros(),
			"PSMAIN",
			"ps_5_0");

		loadMaterialResources(*material);

		MaterialPtr mat = MaterialPtr(material);
		m_materialList.push_back(mat);
	}

	uint32 numMeshes = sdkMesh.GetNumMeshes();
	//m_meshList.resize(numMeshes);
	for (uint32 meshIdx = 0; meshIdx < numMeshes; ++meshIdx)
	{
		MeshDX11Ptr mesh = MeshDX11Ptr(new MeshDX11);
		mesh->createFromSDKMeshFile(sdkMesh, meshIdx);
		m_meshList.push_back(mesh);
		//m_meshList[meshIdx].createFromSDKMeshFile(sdkMesh, meshIdx);// Initialize(device, sdkMesh, meshIdx, generateTangentFrame);
	}
		

	//add submesh list
	for (size_t meshID = 0; meshID < m_meshList.size(); ++meshID)
	{
		const MeshDX11Ptr& mesh = m_meshList[meshID];

		for (size_t subMeshID = 0; subMeshID < mesh->m_subMeshList.size(); ++subMeshID)
		{
			const SubMeshDX11& subMesh = mesh->m_subMeshList[subMeshID];
			m_subMeshList.push_back((SubMeshDX11*)(&subMesh));
		}
	}

	std::sort(m_subMeshList.begin(), m_subMeshList.end(), DepthSortAscendingLess());
}

void ModelDX11::createFromMemory(void* vertexBuffer, int vertexSize, uint32_t vertexCount, void* indexBuffer,
	uint32_t	indexCount, DXGI_FORMAT type)
{
	MeshDX11Ptr mesh = MeshDX11Ptr(new MeshDX11);
	SubMeshDX11* subMesh = mesh->createFromMemory(vertexBuffer, vertexSize, vertexCount, indexBuffer, indexCount, type);
	m_meshList.push_back(mesh);
	m_subMeshList.push_back(subMesh);
}

void ModelDX11::draw()
{
	uint16_t currentMaterialID = -1;
	MeshDX11* currentMesh = nullptr;
	for (size_t i = 0; i < m_subMeshList.size(); ++i)
	{
		SubMeshDX11* subMesh = m_subMeshList[i];
		if (subMesh->m_materialID != currentMaterialID)
		{
			currentMaterialID = subMesh->m_materialID;
			const MaterialPtr& material = m_materialList[currentMaterialID];
			material->bind();
		}

		if (subMesh->m_parent != currentMesh)
		{
			currentMesh = subMesh->m_parent;
			currentMesh->bind();
		}

		subMesh->m_parent->drawSubMesh(subMesh->m_subMeshID);
	}

	//for (size_t meshID = 0; meshID < m_meshList.size(); ++meshID)
	//{
	//	const MeshDX11& mesh = m_meshList[meshID];
	//	mesh.bind();
	//	for (size_t subMeshID = 0; subMeshID < mesh.m_subMeshList.size(); ++subMeshID)
	//	{
	//		const SubMeshDX11& subMesh = mesh.m_subMeshList[subMeshID];
	//		const MaterialDX11& material = m_materialList[subMesh.m_materialID];

	//		material.bind();

	//		mesh.drawSubMesh(subMeshID);

	//		//material.unBind();

	//	}
	//}
}

ModelDX11Ptr 	ModelDX11::createFullScreenQuadModel()
{
	//if (m_fullScreenModelPtr == nullptr)
	{
		Vertex_Pos_UV0 data[4];
		data[0].pos = Vector3(-1, 1, 0);
		data[0].uv0 = Vector2(0, 0);
		data[1].pos = Vector3(1, 1, 0);
		data[1].uv0 = Vector2(1, 0);
		data[2].pos = Vector3(1, -1, 0);
		data[2].uv0 = Vector2(1, 1);
		data[3].pos = Vector3(-1, -1, 0);
		data[3].uv0 = Vector2(0, 1);

		//m_quadMesh->createVertexBuffer(data, sizeof(Vertex_Pos_UV0), 4 * sizeof(Vertex_Pos_UV0),4);

		uint16_t index[6] = { 0,1,2,2,3,0 };

		ModelDX11* model = new ModelDX11;
		model->createFromMemory(data, sizeof(Vertex_Pos_UV0), 4, index, 6);
		return ModelDX11Ptr(model);
	}
	
	//return m_fullScreenModelPtr;
}