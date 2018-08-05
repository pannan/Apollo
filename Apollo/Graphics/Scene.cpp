#include "stdafx.h"
#include "Scene.h"

using namespace Apollo;

Scene::Scene()
{

}

Scene::~Scene()
{

}

void Scene::update()
{

}

void Scene::render()
{
	uint16_t currentMaterialID = -1;

	//�������Ѿ�����������ˣ�������update����
	for each (IRenderable* var in m_visibleRenderableList)
	{

	}


	for (size_t i = 0; i < m_subMeshList.size(); ++i)
	{
		SubMeshDX11* subMesh = m_subMeshList[i];
		if (subMesh->m_materialID != currentMaterialID)
		{
			currentMaterialID = subMesh->m_materialID;
			const MaterialDX11& material = m_materialList[currentMaterialID];
			material.bind();
		}

		if (subMesh->m_parent != currentMesh)
		{
			currentMesh = subMesh->m_parent;
			currentMesh->bind();
		}

		subMesh->m_parent->drawSubMesh(subMesh->m_subMeshID);
	}
}

//void Scene::accept(Visitor* visitor)
//{
//
//}