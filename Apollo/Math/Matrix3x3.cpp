#include "stdafx.h"
#include "Matrix3x3.h"

namespace Apollo
{
	void Matrix3x3::Identity()
	{
		for(int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				m_matrix[i][j] = 0.0f;
				if(i == j)
					m_matrix[i][j] = 1.0f;
			}
		}		
	}

	//正交矩阵定义是，矩阵的每个基向量相互垂直（dot为0）
	//我们的矩阵为行矩阵
	bool Matrix3x3::isOrthogonal()
	{
		for (int i = 0; i < 3; ++i)
		{
			for (int j = i + 1; j < 3; ++j)
			{
				Vector3 v0(m_matrix[i][0],m_matrix[i][1],m_matrix[i][2]);
				Vector3 v1(m_matrix[j][0],m_matrix[j][1],m_matrix[j][2]);
				float dotValue = v0.dot(v1);
				if(dotValue != 0.0f)
					return false;
			}
		}

		return true;
	}

	Matrix3x3& Matrix3x3::transposition()
	{
		std::swap<float,float>(m_matrix[0][1],m_matrix[1][0]);
		std::swap<float, float>(m_matrix[0][2],m_matrix[2][0]);
		std::swap<float, float>(m_matrix[1][2],m_matrix[2][1]);

		return *this;
	}
}

