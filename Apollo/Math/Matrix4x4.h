#pragma once

#include "Vector4.h"
#include "Vector3.h"

namespace Apollo
{
	class Matrix4x4
	{
	public:

		Matrix4x4()
		{
			Identity();
		}

		Matrix4x4(const float i_f11, const float i_f12, const float i_f13, const float i_f14,
			const float i_f21, const float i_f22, const float i_f23, const float i_f24,
			const float i_f31, const float i_f32, const float i_f33, const float i_f34,
			const float i_f41, const float i_f42, const float i_f43, const float i_f44);
		
		void Identity();

		Matrix4x4 inverse();

		//判断是否是正交矩阵
		bool isOrthogonal();

		void makeTransformMatrix(Vector3 offset);
		
		Matrix4x4&		transposition();

		Matrix4x4 operator * (const Matrix4x4& mat) const;

		Matrix4x4 operator / (const float i_fVal);

	protected:

		float determinant();

	public:

	//private:

		union
		{
			struct
			{
				float _11, _12, _13, _14;
				float _21, _22, _23, _24;
				float _31, _32, _33, _34;
				float _41, _42, _43, _44;
			};
			struct { float m_matrix[4][4]; };
		};
	
		//float	m_matrix[4][4];
	};
}