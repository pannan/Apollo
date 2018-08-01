#include "stdafx.h"
#include "Plane.h"
#include "Graphics/AABB.h"

using namespace Apollo;

PlaneAABBIntersectType Plane::intersectWithAABB(const AABB& aabb)
{
	const Vector3 aabbMin = aabb.getMin();
	const Vector3 aabbMax = aabb.getMax();
	//aabb是凸体，找出凸体中离plane最近最远的两个端点，判断和plane的位置关系
	Vector3 min, max;
	if (a >= 0)
	{
		min.m_x = aabbMin.m_x;
		max.m_x = aabbMax.m_x;
	}
	else
	{
		min.m_x = aabbMax.m_x;
		max.m_x = aabbMin.m_x;
	}

	if (b >= 0)
	{
		min.m_y = aabbMin.m_y;
		max.m_y = aabbMax.m_y;
	}
	else
	{
		min.m_y = aabbMax.m_y;
		max.m_y = aabbMin.m_y;
	}
		
	if (c >= 0)
	{
		min.m_z = aabbMin.m_z;
		max.m_z = aabbMax.m_z;
	}
	else
	{
		min.m_z = aabbMax.m_z;
		max.m_z = aabbMin.m_z;
	}

	if (distance(min) > 0)
		return PAIT_INSIDE;
	else if (distance(max) < 0)
		return PAIT_OUTSIDE;

	return PAIT_INTERSECT;
}