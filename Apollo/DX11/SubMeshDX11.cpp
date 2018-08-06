#include "stdafx.h"
#include "SubMeshDX11.h"
#include "RendererDX11.h"

using namespace  Apollo;

void SubMeshDX11::setMaterialID(uint16_t id)
{
	m_materialID = id;
	IRenderable::setMaterialID(id);
}

void SubMeshDX11::render()
{
	RendererDX11::getInstance().drawIndexed(m_indexCount, m_indexStart, m_vertexStart);
}