#include "stdafx.h"
#include "Vector4.h"
#include "Matrix4x4.h"

namespace Apollo
{
	float Vector4::dot(const Vector4 vec4)
	{
		return m_w * vec4.m_w + m_x * vec4.m_x + m_y * vec4.m_y + m_z * vec4.m_z;
	}

	float Vector4::dot(float x,float y,float z,float w)
	{
		return m_w * w + m_x * x + m_y * y + m_z * z;
	}

	Vector4 Vector4::operator * (const Matrix4x4& mat4x4)
	{
		const float x = dot(mat4x4.m_matrix[0][0],mat4x4.m_matrix[1][0],mat4x4.m_matrix[2][0],mat4x4.m_matrix[3][0]);
		const float y = dot(mat4x4.m_matrix[0][1],mat4x4.m_matrix[1][1],mat4x4.m_matrix[2][1],mat4x4.m_matrix[3][1]);
		const float z = dot(mat4x4.m_matrix[0][2],mat4x4.m_matrix[1][2],mat4x4.m_matrix[2][2],mat4x4.m_matrix[3][2]);
		const float w = dot(mat4x4.m_matrix[0][3],mat4x4.m_matrix[1][3],mat4x4.m_matrix[2][3],mat4x4.m_matrix[3][3]);
		return Vector4(x,y,z,w);
	}
}