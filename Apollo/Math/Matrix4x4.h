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
		
		void Identity();

		Matrix4x4 inverse();

		//判断是否是正交矩阵
		bool isOrthogonal();

		void makeTransformMatrix(Vector3 offset);
		
		Matrix4x4&		transposition();

		Matrix4x4 operator * (const Matrix4x4& mat) const;

	//private:

	
		float	m_matrix[4][4];
	};
}