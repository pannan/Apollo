#include "stdafx.h"
#include "Quaternion.h"
#include "Matrix3x3.h"
#include "Matrix4x4.h"

namespace Apollo
{
	Quaternion::Quaternion(): m_x(0.0f), m_y(0.0f), m_z(0.0f), m_w(1.0f)
	{

	}

	Quaternion::Quaternion(float x,float y,float z,float w) : m_x(x), m_y(y), m_z(z), m_w(w)
	{

	}

	Quaternion::~Quaternion()
	{

	}

	Quaternion Quaternion::operator* (float f) const
	{
		return Quaternion(f*m_x,f*m_y,f*m_z,f*m_w);
	}

	float  Quaternion::norm () const
	{
		return m_w*m_w+m_x*m_x+m_y*m_y+m_z*m_z;
	}

	float Quaternion::normalize()
	{
		float len = norm();
		float factor = 1.0f / sqrtf(len);
		*this = *this * factor;
		return len;
	}

	void Quaternion::createFromAxisAngle(const float &in_x, const float &in_y, const float &in_z, const float &in_degrees)
	{

		float angle = float((in_degrees / 180.0f) * PI);
		float result = float(sin(angle/2.0f));
		m_w = float(cos(angle/2.0f));

		// Calculate the x, y and z of the quaternion
		m_x = float(in_x * result);
		m_y = float(in_y * result);
		m_z = float(in_z * result);
	}

	void Quaternion::toMatrix(Matrix4x4& mat)
	{		
		// First row
		mat.m_matrix[0][0] = 1.0f - 2.0f * ( m_y * m_y + m_z * m_z );  
		mat.m_matrix[0][1] = 2.0f * ( m_x * m_y - m_w * m_z );  
		mat.m_matrix[0][2] = 2.0f * ( m_x * m_z + m_w * m_y );  
		mat.m_matrix[0][3] = 0.0f;  

		// Second row
		mat.m_matrix[1][0] = 2.0f * ( m_x * m_y + m_w * m_z );  
		mat.m_matrix[1][1] = 1.0f - 2.0f * ( m_x * m_x + m_z * m_z );  
		mat.m_matrix[1][2] = 2.0f * ( m_y * m_z - m_w * m_x );  
		mat.m_matrix[1][3] = 0.0f;  

		// Third row
		mat.m_matrix[2][0] = 2.0f * ( m_x * m_z - m_w * m_y );  
		mat.m_matrix[2][1] = 2.0f * ( m_y * m_z + m_w * m_x );  
		mat.m_matrix[2][2] = 1.0f - 2.0f * ( m_x * m_x + m_y * m_y );  
		mat.m_matrix[2][3] = 0.0f;  

		// Fourth row
		mat.m_matrix[3][0] = 0;  
		mat.m_matrix[3][1] = 0;  
		mat.m_matrix[3][2] = 0;  
		mat.m_matrix[3][3] = 1.0f;
		
	}

	void Quaternion::fromRotationMatrix(const Matrix3x3& mat)
	{
		// Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
		// article "Quaternion Calculus and Fast Animation".

		float fTrace = mat.m_matrix[0][0]+mat.m_matrix[1][1]+mat.m_matrix[2][2];
		float fRoot;

		if ( fTrace > 0.0 )
		{
			// |w| > 1/2, may as well choose w > 1/2
			fRoot = sqrtf(fTrace + 1.0);  // 2w
			m_w = 0.5*fRoot;
			fRoot = 0.5/fRoot;  // 1/(4w)
			m_x = (mat.m_matrix[2][1]-mat.m_matrix[1][2])*fRoot;
			m_y = (mat.m_matrix[0][2]-mat.m_matrix[2][0])*fRoot;
			m_z = (mat.m_matrix[1][0]-mat.m_matrix[0][1])*fRoot;
		}
		else
		{
			// |w| <= 1/2
			static size_t s_iNext[3] = { 1, 2, 0 };
			size_t i = 0;
			if ( mat.m_matrix[1][1] > mat.m_matrix[0][0] )
				i = 1;
			if ( mat.m_matrix[2][2] > mat.m_matrix[i][i] )
				i = 2;
			size_t j = s_iNext[i];
			size_t k = s_iNext[j];

			fRoot = sqrtf(mat.m_matrix[i][i]-mat.m_matrix[j][j]-mat.m_matrix[k][k] + 1.0);
			float* apkQuat[3] = { &m_x, &m_y, &m_z };
			*apkQuat[i] = 0.5*fRoot;
			fRoot = 0.5/fRoot;
			m_w = (mat.m_matrix[k][j]-mat.m_matrix[j][k])*fRoot;
			*apkQuat[j] = (mat.m_matrix[j][i]+mat.m_matrix[i][j])*fRoot;
			*apkQuat[k] = (mat.m_matrix[k][i]+mat.m_matrix[i][k])*fRoot;
		}
	}

	Quaternion Quaternion::operator *(const Quaternion &q)
	{
		Quaternion r;

		r.m_w = m_w*q.m_w - m_x*q.m_x - m_y*q.m_y - m_z*q.m_z;
		r.m_x = m_w*q.m_x + m_x*q.m_w + m_y*q.m_z - m_z*q.m_y;
		r.m_y = m_w*q.m_y + m_y*q.m_w + m_z*q.m_x - m_x*q.m_z;
		r.m_z = m_w*q.m_z + m_z*q.m_w + m_x*q.m_y - m_y*q.m_x;

		return r;
	}
}