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

	Matrix4x4 Matrix4x4::inverse()
	{
		float fA0 = m_afEntry[0] * m_afEntry[5] - m_afEntry[1] * m_afEntry[4];
		float fA1 = m_afEntry[0] * m_afEntry[6] - m_afEntry[2] * m_afEntry[4];
		float fA2 = m_afEntry[0] * m_afEntry[7] - m_afEntry[3] * m_afEntry[4];
		float fA3 = m_afEntry[1] * m_afEntry[6] - m_afEntry[2] * m_afEntry[5];
		float fA4 = m_afEntry[1] * m_afEntry[7] - m_afEntry[3] * m_afEntry[5];
		float fA5 = m_afEntry[2] * m_afEntry[7] - m_afEntry[3] * m_afEntry[6];
		float fB0 = m_afEntry[8] * m_afEntry[13] - m_afEntry[9] * m_afEntry[12];
		float fB1 = m_afEntry[8] * m_afEntry[14] - m_afEntry[10] * m_afEntry[12];
		float fB2 = m_afEntry[8] * m_afEntry[15] - m_afEntry[11] * m_afEntry[12];
		float fB3 = m_afEntry[9] * m_afEntry[14] - m_afEntry[10] * m_afEntry[13];
		float fB4 = m_afEntry[9] * m_afEntry[15] - m_afEntry[11] * m_afEntry[13];
		float fB5 = m_afEntry[10] * m_afEntry[15] - m_afEntry[11] * m_afEntry[14];

		float fDet = fA0*fB5 - fA1*fB4 + fA2*fB3 + fA3*fB2 - fA4*fB1 + fA5*fB0;
		//if ( Math<Real>::FAbs(fDet) <= Math<Real>::EPSILON )
		//    return Matrix4::ZERO;

		Matrix4f kInv;
		kInv(0, 0) = +m_afEntry[5] * fB5 - m_afEntry[6] * fB4 + m_afEntry[7] * fB3;
		kInv(1, 0) = -m_afEntry[4] * fB5 + m_afEntry[6] * fB2 - m_afEntry[7] * fB1;
		kInv(2, 0) = +m_afEntry[4] * fB4 - m_afEntry[5] * fB2 + m_afEntry[7] * fB0;
		kInv(3, 0) = -m_afEntry[4] * fB3 + m_afEntry[5] * fB1 - m_afEntry[6] * fB0;
		kInv(0, 1) = -m_afEntry[1] * fB5 + m_afEntry[2] * fB4 - m_afEntry[3] * fB3;
		kInv(1, 1) = +m_afEntry[0] * fB5 - m_afEntry[2] * fB2 + m_afEntry[3] * fB1;
		kInv(2, 1) = -m_afEntry[0] * fB4 + m_afEntry[1] * fB2 - m_afEntry[3] * fB0;
		kInv(3, 1) = +m_afEntry[0] * fB3 - m_afEntry[1] * fB1 + m_afEntry[2] * fB0;
		kInv(0, 2) = +m_afEntry[13] * fA5 - m_afEntry[14] * fA4 + m_afEntry[15] * fA3;
		kInv(1, 2) = -m_afEntry[12] * fA5 + m_afEntry[14] * fA2 - m_afEntry[15] * fA1;
		kInv(2, 2) = +m_afEntry[12] * fA4 - m_afEntry[13] * fA2 + m_afEntry[15] * fA0;
		kInv(3, 2) = -m_afEntry[12] * fA3 + m_afEntry[13] * fA1 - m_afEntry[14] * fA0;
		kInv(0, 3) = -m_afEntry[9] * fA5 + m_afEntry[10] * fA4 - m_afEntry[11] * fA3;
		kInv(1, 3) = +m_afEntry[8] * fA5 - m_afEntry[10] * fA2 + m_afEntry[11] * fA1;
		kInv(2, 3) = -m_afEntry[8] * fA4 + m_afEntry[9] * fA2 - m_afEntry[11] * fA0;
		kInv(3, 3) = +m_afEntry[8] * fA3 - m_afEntry[9] * fA1 + m_afEntry[10] * fA0;

		float fInvDet = ((float)1.0) / fDet;
		for (int iRow = 0; iRow < 4; iRow++)
		{
			for (int iCol = 0; iCol < 4; iCol++)
				kInv(iRow, iCol) *= fInvDet;
		}

		return(kInv);
	}
}