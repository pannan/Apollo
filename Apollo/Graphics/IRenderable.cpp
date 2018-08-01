#include "stdafx.h"
#include "IRenderable.h"

using namespace  Apollo;

void IRenderable::setMaterialID(uint16_t id)
{
	uint32_t temp = id;
	//把低16位清零
	m_handle &= 0xffff0000;
	//在低16位赋值
	m_handle |= temp;
}