#include "stdafx.h"
#include "AABB.h"

using namespace Apollo;
using namespace std;

AABB::AABB()
{
	m_min = Vector3(MAX_FLOAT, MAX_FLOAT, MAX_FLOAT);
	m_max = Vector3(MIN_FLOAT,MIN_FLOAT,MIN_FLOAT);

	ZeroMemory(m_corner, sizeof(Vector3) * 8);

	m_isDirty = false;
}

void AABB::merge(const Vector3& pos)
{
	bool xInside = (pos.m_x >= m_min.m_x) && (pos.m_x <= m_max.m_x);
	bool yInside = (pos.m_y >= m_min.m_y) && (pos.m_y <= m_max.m_y);
	bool zInside = (pos.m_z>= m_min.m_z) && (pos.m_z <= m_max.m_z);

	//pos在aabb内，不会增大aabb
	if (xInside && yInside && zInside)
		return;

	m_isDirty = true;

	if (pos.m_x < m_min.m_x)
		m_min.m_x = pos.m_x;
	else if (pos.m_x > m_max.m_x)
		m_max.m_x = pos.m_x;

	if (pos.m_y < m_min.m_y)
		m_min.m_y = pos.m_y;
	else if (pos.m_y > m_max.m_y)
		m_max.m_y = pos.m_y;

	if (pos.m_z < m_min.m_z)
		m_min.m_z = pos.m_z;
	else if (pos.m_z > m_max.m_z)
		m_max.m_z = pos.m_z;
}

const Vector3*		AABB::getCotner() 
{
	if (m_isDirty == false)
		return m_corner;

	m_corner[ACI_FRONT_LT] = Vector3(m_min.m_x,m_max.m_y,m_min.m_z);
	m_corner[ACI_FRONT_RT] = Vector3(m_max.m_x, m_max.m_y, m_min.m_z);
	m_corner[ACI_FRONT_RB] = Vector3(m_max.m_x, m_min.m_y, m_min.m_z);
	m_corner[ACI_FRONT_LB] = Vector3(m_min.m_x, m_min.m_y, m_min.m_z);

	m_corner[ACI_BACK_LT] = Vector3(m_max.m_x, m_max.m_y, m_max.m_z);
	m_corner[ACI_BACK_RT] = Vector3(m_min.m_x, m_max.m_y, m_max.m_z);
	m_corner[ACI_FRONT_RB] = Vector3(m_min.m_x, m_min.m_y, m_max.m_z);
	m_corner[ACI_FRONT_LB] = Vector3(m_max.m_x, m_min.m_y, m_max.m_z);

	return m_corner;
}