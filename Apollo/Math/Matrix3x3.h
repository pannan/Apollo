#pragma once

#include "Vector3.h"
namespace Apollo
{
	class Matrix3x3
	{
	public:

		Matrix3x3()
		{
			Identity();
		}


		void Identity();

		//�ж��Ƿ�����������
		bool isOrthogonal();

		Matrix3x3&		invert();

		Matrix3x3&		transposition();

		float	m_matrix[3][3];	
	};
}