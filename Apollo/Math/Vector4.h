#pragma once


namespace Apollo
{
	class Matrix4x4;

	class Vector4
	{
	public:

		Vector4():m_w(0.0f),m_x(0.0f),m_y(0.0f),m_z(0.0f){}	
		Vector4(float x,float y,float z,float w):m_w(w),m_x(x),m_y(y),m_z(z){}
			

		float dot(const Vector4 vec4);
		float dot(float x,float y,float z,float w);

		Vector4 operator - (const Vector4& vec4) const
		{
			return Vector4(m_x - vec4.m_x,m_y - vec4.m_y,m_z - vec4.m_z,m_w - vec4.m_w);
		}

		Vector4 operator + (const Vector4& vec4) const
		{
			return Vector4(m_x + vec4.m_x,m_y + vec4.m_y,m_z + vec4.m_z,m_w + vec4.m_w);
		}

		Vector4 operator * (const Matrix4x4& mat4x4);

		Vector4 operator * (float f) const
		{
			return Vector4(m_x * f,m_y * f,m_z * f,m_w * f);
		} 

		Vector4 operator / (float f) const
		{
			return Vector4(m_x / f,m_y / f,m_z / f,m_w / f);
		}
		
		float m_x;
		float m_y;
		float m_z;
		float m_w;
	};
}