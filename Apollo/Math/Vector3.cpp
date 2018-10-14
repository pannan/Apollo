#include "stdafx.h"
#include "Matrix3x3.h"
#include "Vector3.h"

namespace Apollo
{
	Vector3	Vector3::s_ZeroVec = Vector3(0.0,0.0,0.0);
	Vector3	Vector3::s_OneVec = Vector3(1.0,1.0,1.0);
	Vector3	Vector3::s_XVec = Vector3(1.0,0.0,0.0);
	Vector3	Vector3::s_YVec = Vector3(0.0,1.0,0.0);
	Vector3	Vector3::s_ZVec = Vector3(0.0,0.0,1.0);

	Vector3 Vector3::operator * (const Matrix3x3& mat3x3)
	{
		Vector3 v0(mat3x3.m_matrix[0][0],mat3x3.m_matrix[0][1],mat3x3.m_matrix[0][2]);
		Vector3 v1(mat3x3.m_matrix[1][0],mat3x3.m_matrix[1][1],mat3x3.m_matrix[1][2]);
		Vector3 v2(mat3x3.m_matrix[2][0],mat3x3.m_matrix[2][1],mat3x3.m_matrix[2][2]);
		return Vector3(dot(v0),dot(v1),dot(v2));
	}

	float Vector3::length()
	{
		float t = m_x * m_x + m_y * m_y + m_z * m_z;
		t = sqrtf(t);
		return t;
	}

	Vector3&	Vector3::normalize()
	{
		float t = m_x * m_x + m_y * m_y + m_z * m_z;
		t = sqrtf(t);
		t = 1.0f / t;
		m_x *= t;
		m_y *= t;
		m_z *= t;

		return *this;
	}

	/*
	i*i = 0 j*j = 0 k*k = 0
	i*j = k j*k = i k*i = j
	j*i = -k k*j = -i i*k = -j

	(Uxi + Uyj + Uzk)x(Vxi + Vyj + Vzk) = UxVx(i*i) + UxVy(i*j) + UxVz(i*k) + UyVx(j*i) + UyVy(j*j) + UyVz(j*k) + UzVx(k*i) + UzVy(k*j) + UzVz(k*k)
	=UxVyk - UxVzj - UyVxk + UyVzi +  UzVxj - UzVyi
	=(UyVz - UzVy)i + (UzVx - UxVz)j + (UxVy - UyVx)k
	*/
	Vector3 Vector3::corss(const Vector3& vec)
	{
		Vector3 v(	m_y * vec.m_z - m_z * vec.m_y,
					m_z * vec.m_x - m_x * vec.m_z,
					m_x * vec.m_y - m_y * vec.m_x);
		return v;
	}
}