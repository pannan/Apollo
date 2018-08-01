#include "stdafx.h"
#include "Frustum.h"
#include "AABB.h"
using namespace Apollo;

void Frustum::extractionPlane(const Matrix4x4& mat)
{
	m_frustumPlane[FP_LEFT].a = mat.m_matrix[0][3] + mat.m_matrix[0][0];
	m_frustumPlane[FP_LEFT].b = mat.m_matrix[1][3] + mat.m_matrix[1][0];
	m_frustumPlane[FP_LEFT].c = mat.m_matrix[2][3] + mat.m_matrix[2][0];
	m_frustumPlane[FP_LEFT].d = mat.m_matrix[3][3] + mat.m_matrix[3][0];

	m_frustumPlane[FP_RIGHT].a = mat.m_matrix[0][3] - mat.m_matrix[0][0];
	m_frustumPlane[FP_RIGHT].b = mat.m_matrix[1][3] - mat.m_matrix[1][0];
	m_frustumPlane[FP_RIGHT].c = mat.m_matrix[2][3] - mat.m_matrix[2][0];
	m_frustumPlane[FP_RIGHT].d = mat.m_matrix[3][3] - mat.m_matrix[3][0];

	m_frustumPlane[FP_BOTTOM].a = mat.m_matrix[0][3] + mat.m_matrix[0][1];
	m_frustumPlane[FP_BOTTOM].b = mat.m_matrix[1][3] + mat.m_matrix[1][1];
	m_frustumPlane[FP_BOTTOM].c = mat.m_matrix[2][3] + mat.m_matrix[2][1];
	m_frustumPlane[FP_BOTTOM].d = mat.m_matrix[3][3] + mat.m_matrix[3][1];

	m_frustumPlane[FP_TOP].a = mat.m_matrix[0][3] - mat.m_matrix[0][1];
	m_frustumPlane[FP_TOP].b = mat.m_matrix[1][3] - mat.m_matrix[1][1];
	m_frustumPlane[FP_TOP].c = mat.m_matrix[2][3] - mat.m_matrix[2][1];
	m_frustumPlane[FP_TOP].d = mat.m_matrix[3][3] - mat.m_matrix[3][1];

	m_frustumPlane[FP_NEAR].a = mat.m_matrix[0][2];
	m_frustumPlane[FP_NEAR].b = mat.m_matrix[1][2];
	m_frustumPlane[FP_NEAR].c = mat.m_matrix[2][2];
	m_frustumPlane[FP_NEAR].d = mat.m_matrix[3][2];

	m_frustumPlane[FP_FAR].a = mat.m_matrix[0][3] - mat.m_matrix[0][2];
	m_frustumPlane[FP_FAR].b = mat.m_matrix[1][3] - mat.m_matrix[1][2];
	m_frustumPlane[FP_FAR].c = mat.m_matrix[2][3] - mat.m_matrix[2][2];
	m_frustumPlane[FP_FAR].d = mat.m_matrix[3][3] - mat.m_matrix[3][2];
}

bool Frustum::isVisible(const AABB& aabb)
{
	const Vector3* corner = aabb.getCotner();

	for (int i = 0;  i < 8; ++i)
	{

	}
}