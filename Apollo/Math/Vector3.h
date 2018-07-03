#pragma once

namespace Apollo
{
	class Matrix3x3;

	class Vector3
	{
	public:

		Vector3(float x,float y,float z) : m_x(x),m_y(y),m_z(z){}
		Vector3():m_x(0.0f),m_y(0.0f),m_z(0.0f){}

		float dot(const Vector3& vec3)
		{
			return m_x * vec3.m_x + m_y * vec3.m_y + m_z * vec3.m_z;
		}

		Vector3 operator - (const Vector3& vec3)
		{
			return Vector3(m_x - vec3.m_x,m_y - vec3.m_y,m_z - vec3.m_z);
		}

		Vector3 operator + (const Vector3& vec3)
		{
			return Vector3(m_x + vec3.m_x,m_y + vec3.m_y,m_z + vec3.m_z);
		}

		Vector3 operator - ()
		{
			return Vector3(-m_x,-m_y,-m_z);
		}

		Vector3 operator * (const float& f)const
		{
			return Vector3(m_x *f,m_y *f,m_z * f);
		}

		Vector3 operator *= (const float& f)
		{
			m_x *= f;
			m_y *= f;
			m_z *= f;

			return *this;
		}

		Vector3 operator - (const Vector3& vec) const
		{
			return Vector3(m_x - vec.m_x,m_y - vec.m_y,m_z - vec.m_z);
		}

		Vector3& normalize();

		Vector3 corss(const Vector3& vec);

		Vector3 operator * (const Matrix3x3& mat3x3);

		float m_x;
		float m_y;
		float m_z;
		
		static Vector3 s_XVec;
		static Vector3 s_YVec;
		static Vector3 s_ZVec;
		static Vector3 s_ZeroVec;
		static Vector3 s_OneVec;
	};
}