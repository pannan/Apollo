#include "stdafx.h"
#include "IRenderable.h"

using namespace  Apollo;

void IRenderable::setMaterialID(uint16_t id)
{
	uint32_t temp = id;
	//�ѵ�16λ����
	m_handle &= 0xffff0000;
	//�ڵ�16λ��ֵ
	m_handle |= temp;
}