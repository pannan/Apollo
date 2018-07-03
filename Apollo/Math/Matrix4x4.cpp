#include "stdafx.h"
#include "Matrix4x4.h"

namespace Apollo
{
	void Matrix4x4::Identity()
	{
		for(int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				m_matrix[i][j] = 0.0f;
				if(i == j)
					m_matrix[i][j] = 1.0f;
			}
		}	
	}

	//正交矩阵定义是，矩阵的每个基向量相互垂直（dot为0）
	//我们的矩阵为行矩阵
	bool Matrix4x4::isOrthogonal()
	{
		for (int i = 0; i < 4; ++i)
		{
			for (int j = i + 1; j < 4; ++j)
			{
				Vector4 v0(m_matrix[i][0],m_matrix[i][1],m_matrix[i][2],m_matrix[i][3]);
				Vector4 v1(m_matrix[j][0],m_matrix[j][1],m_matrix[j][2],m_matrix[j][3]);
				float dotValue = v0.dot(v1);
				if(dotValue != 0.0f)
					return false;
			}
		}

		return true;
	}

	void Matrix4x4::makeTransformMatrix(Vector3 offset)
	{
		Identity();
		m_matrix[3][0] = offset.m_x;
		m_matrix[3][1] = offset.m_y;
		m_matrix[3][2] = offset.m_z;	
	}

	Matrix4x4& Matrix4x4::transposition()
	{
		std::swap<float,float>(m_matrix[0][1],m_matrix[1][0]);
		std::swap<float,float>(m_matrix[0][2],m_matrix[2][0]);
		std::swap<float, float>(m_matrix[1][2],m_matrix[2][1]);
		std::swap<float, float>(m_matrix[3][0],m_matrix[0][3]);
		std::swap<float, float>(m_matrix[3][1],m_matrix[1][3]);
		std::swap<float, float>(m_matrix[3][2],m_matrix[2][3]);

		return *this;
	}

	Matrix4x4 Matrix4x4::operator * (const Matrix4x4& mat) const
	{
		Vector4 rowVec0(m_matrix[0][0],m_matrix[0][1],m_matrix[0][2],m_matrix[0][3]);
		Vector4 rowVec1(m_matrix[1][0],m_matrix[1][1],m_matrix[1][2],m_matrix[1][3]);
		Vector4 rowVec2(m_matrix[2][0],m_matrix[2][1],m_matrix[2][2],m_matrix[2][3]);
		Vector4 rowVec3(m_matrix[3][0],m_matrix[3][1],m_matrix[3][2],m_matrix[3][3]);

		Vector4 columnVec0(mat.m_matrix[0][0],mat.m_matrix[1][0],mat.m_matrix[2][0],mat.m_matrix[3][0]);
		Vector4 columnVec1(mat.m_matrix[0][1],mat.m_matrix[1][1],mat.m_matrix[2][1],mat.m_matrix[3][1]);
		Vector4 columnVec2(mat.m_matrix[0][2],mat.m_matrix[1][2],mat.m_matrix[2][2],mat.m_matrix[3][2]);
		Vector4 columnVec3(mat.m_matrix[0][3],mat.m_matrix[1][3],mat.m_matrix[2][3],mat.m_matrix[3][3]);


		Matrix4x4 newMat;
		newMat.m_matrix[0][0] = rowVec0.dot(columnVec0);
		newMat.m_matrix[0][1] = rowVec0.dot(columnVec1);
		newMat.m_matrix[0][2] = rowVec0.dot(columnVec2);
		newMat.m_matrix[0][3] = rowVec0.dot(columnVec3);

		newMat.m_matrix[1][0] = rowVec1.dot(columnVec0);
		newMat.m_matrix[1][1] = rowVec1.dot(columnVec1);
		newMat.m_matrix[1][2] = rowVec1.dot(columnVec2);
		newMat.m_matrix[1][3] = rowVec1.dot(columnVec3);

		newMat.m_matrix[2][0] = rowVec2.dot(columnVec0);
		newMat.m_matrix[2][1] = rowVec2.dot(columnVec1);
		newMat.m_matrix[2][2] = rowVec2.dot(columnVec2);
		newMat.m_matrix[2][3] = rowVec2.dot(columnVec3);

		newMat.m_matrix[3][0] = rowVec3.dot(columnVec0);
		newMat.m_matrix[3][1] = rowVec3.dot(columnVec1);
		newMat.m_matrix[3][2] = rowVec3.dot(columnVec2);
		newMat.m_matrix[3][3] = rowVec3.dot(columnVec3);

		return newMat;
	}
}