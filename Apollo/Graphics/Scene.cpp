#include "stdafx.h"
#include "Scene.h"
#include "IRenderable.h"

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

void Scene::addRenderable(IRenderable* renderable)
{
	m_visibleRenderableList.push_back(renderable);
}

void Scene::render()
{
	uint16_t currentMaterialID = -1;

	//�������Ѿ�����������ˣ�������update����
	for each (IRenderable* var in m_visibleRenderableList)
	{
		uint16_t materialID = var->getMaterialID();
		if (materialID != currentMaterialID)
		{
			currentMaterialID = materialID;
			const MaterialPtr& material = m_materialList[materialID];
			material->bind();
		}

		var->render();
	}
}

//void Scene::accept(Visitor* visitor)
//{
//
//}