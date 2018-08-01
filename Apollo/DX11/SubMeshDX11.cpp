#include "stdafx.h"
#include "SubMeshDX11.h"

using namespace  Apollo;

void SubMeshDX11::setMaterialID(uint16_t id)
{
	m_materialID = id;
	IRenderable::setMaterialID(id);
}