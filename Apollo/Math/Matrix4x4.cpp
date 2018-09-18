#include "stdafx.h"
#include "Matrix4x4.h"

namespace Apollo
{

	inline Matrix4x4::Matrix4x4(const float i_f11, const float i_f12, const float i_f13, const float i_f14,
		const float i_f21, const float i_f22, const float i_f23, const float i_f24,
		const float i_f31, const float i_f32, const float i_f33, const float i_f34,
		const float i_f41, const float i_f42, const float i_f43, const float i_f44)
		: _11(i_f11), _12(i_f12), _13(i_f13), _14(i_f14),
		_21(i_f21), _22(i_f22), _23(i_f23), _24(i_f24),
		_31(i_f31), _32(i_f32), _33(i_f33), _34(i_f34),
		_41(i_f41), _42(i_f42), _43(i_f43), _44(i_f44)
	{}

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

	static inline float fDeterminant2x2(const float i_fA, const float i_fB, const float i_fC, const float i_fD)
	{
		return i_fA * i_fD - i_fB * i_fC;
	}

	static float fDeterminant3x3(const float i_fA1, const float i_fA2, const float i_fA3,
		const float i_fB1, const float i_fB2, const float i_fB3,
		const float i_fC1, const float i_fC2, const float i_fC3)
	{
		// src: http://www.acm.org/pubs/tog/GraphicsGems/gems/MatrixInvert.c
		return i_fA1 * fDeterminant2x2(i_fB2, i_fB3, i_fC2, i_fC3)
			- i_fB1 * fDeterminant2x2(i_fA2, i_fA3, i_fC2, i_fC3)
			+ i_fC1 * fDeterminant2x2(i_fA2, i_fA3, i_fB2, i_fB3);
	}

	float Matrix4x4::determinant()
	{
		// src: http://www.acm.org/pubs/tog/GraphicsGems/gems/MatrixInvert.c
		return _11 * fDeterminant3x3(_22, _32, _42, _23, _33, _43, _24, _34, _44)
			- _12 * fDeterminant3x3(_21, _31, _41, _23, _33, _43, _24, _34, _44)
			+ _13 * fDeterminant3x3(_21, _31, _41, _22, _32, _42, _24, _34, _44)
			- _14 * fDeterminant3x3(_21, _31, _41, _22, _32, _42, _23, _33, _43);
	}

	static float fMinorDeterminant(const Matrix4x4 &i_matMatrix, const uint32 i_iRow, const uint32 i_iColumn)
	{
		// src: http://www.codeproject.com/csharp/Matrix.asp
		float fMat3x3[3][3];

		for (uint32 r = 0, m = 0; r < 4; ++r)
		{
			if (r == i_iRow)
				continue;

			for (uint32 c = 0, n = 0; c < 4; ++c)
			{
				if (c == i_iColumn)
					continue;

				fMat3x3[m][n] = i_matMatrix.m_matrix[r][c];

				++n;
			}

			++m;
		}

		return fDeterminant3x3(fMat3x3[0][0], fMat3x3[0][1], fMat3x3[0][2],
			fMat3x3[1][0], fMat3x3[1][1], fMat3x3[1][2], fMat3x3[2][0], fMat3x3[2][1], fMat3x3[2][2]);
	}

	static Matrix4x4 matAdjoint(const Matrix4x4 *i_pMat)
	{
		// src: http://www.codeproject.com/csharp/Matrix.asp
		Matrix4x4 matReturn;
		for (uint32 r = 0; r < 4; ++r)
		{
			for (uint32 c = 0; c < 4; ++c)
				matReturn.m_matrix[c][r] = powf(-1.0f, (float)(r + c)) * fMinorDeterminant(*i_pMat, r, c);
		}
		return matReturn;
	}

	Matrix4x4 Matrix4x4::inverse()
	{
		const float fDeterminant = determinant();
		if (fabsf(fDeterminant) < FLT_EPSILON)
			return *this;

		return matAdjoint(this) / fDeterminant;
	}

	inline Matrix4x4 Matrix4x4::operator /(const float i_fVal)
	{
		const float fInvVal = 1.0f / i_fVal;
		return Matrix4x4(_11 * fInvVal, _12 * fInvVal, _13 * fInvVal, _14 * fInvVal,
			_21 * fInvVal, _22 * fInvVal, _23 * fInvVal, _24 * fInvVal,
			_31 * fInvVal, _32 * fInvVal, _33 * fInvVal, _34 * fInvVal,
			_41 * fInvVal, _42 * fInvVal, _43 * fInvVal, _44 * fInvVal);
	}

	//Matrix4x4 Matrix4x4::inverse()
	//{
	//	float fA0 = m_matrix[0][0] * m_matrix[1][1] - m_matrix[0][1] * m_matrix[1][0];
	//	float fA1 = m_matrix[0][0] * m_matrix[1][2] - m_matrix[0][2] * m_matrix[1][0];
	//	float fA2 = m_matrix[0][0] * m_matrix[1][3] - m_matrix[0][3] * m_matrix[1][0];
	//	float fA3 = m_matrix[0][1] * m_matrix[1][2] - m_matrix[0][2] * m_matrix[1][1];
	//	float fA4 = m_matrix[0][1] * m_matrix[1][3] - m_matrix[0][3] * m_matrix[1][1];
	//	float fA5 = m_matrix[0][2] * m_matrix[1][3] - m_matrix[0][3] * m_matrix[1][2];
	//	float fB0 = m_matrix[2][0] * m_matrix[3][1] - m_matrix[2][1] * m_matrix[3][0];
	//	float fB1 = m_matrix[2][0] * m_matrix[3][2] - m_matrix[2][2] * m_matrix[3][0];
	//	float fB2 = m_matrix[2][0] * m_matrix[3][3] - m_matrix[2][3] * m_matrix[3][0];
	//	float fB3 = m_matrix[2][1] * m_matrix[3][2] - m_matrix[2][2] * m_matrix[3][1];
	//	float fB4 = m_matrix[2][1] * m_matrix[3][3] - m_matrix[2][3] * m_matrix[3][1];
	//	float fB5 = m_matrix[2][2] * m_matrix[3][3] - m_matrix[2][3] * m_matrix[3][2];

	//	float fDet = fA0*fB5 - fA1*fB4 + fA2*fB3 + fA3*fB2 - fA4*fB1 + fA5*fB0;
	//	//if ( Math<Real>::FAbs(fDet) <= Math<Real>::EPSILON )
	//	//    return Matrix4::ZERO;

	//	Matrix4x4 kInv;
	//	kInv.m_matrix[0][0] = +m_matrix[1][1] * fB5 - m_matrix[1][2] * fB4 + m_matrix[1][3] * fB3;
	//	kInv.m_matrix[1][0] = -m_matrix[1][0] * fB5 + m_matrix[1][2] * fB2 - m_matrix[1][3] * fB1;
	//	kInv.m_matrix[2][0] = +m_matrix[1][0] * fB4 - m_matrix[1][1] * fB2 + m_matrix[1][3] * fB0;
	//	kInv.m_matrix[3][0] = -m_matrix[1][0] * fB3 + m_matrix[1][1] * fB1 - m_matrix[1][2] * fB0;
	//	kInv.m_matrix[0][1] = -m_matrix[0][1] * fB5 + m_matrix[0][2] * fB4 - m_matrix[0][3] * fB3;
	//	kInv.m_matrix[1][1] = +m_matrix[0][0] * fB5 - m_matrix[0][2] * fB2 + m_matrix[0][3] * fB1;
	//	kInv.m_matrix[2][1] = -m_matrix[0][0] * fB4 + m_matrix[0][1] * fB2 - m_matrix[0][3] * fB0;
	//	kInv.m_matrix[3][1] = +m_matrix[0][0] * fB3 - m_matrix[0][1] * fB1 + m_matrix[0][2] * fB0;
	//	kInv.m_matrix[0][2] = +m_matrix[3][1] * fA5 - m_matrix[3][2] * fA4 + m_matrix[3][3] * fA3;
	//	kInv.m_matrix[1][2] = -m_matrix[3][0] * fA5 + m_matrix[3][2] * fA2 - m_matrix[3][3] * fA1;
	//	kInv.m_matrix[2][2] = +m_matrix[3][0] * fA4 - m_matrix[3][1] * fA2 + m_matrix[3][3] * fA0;
	//	kInv.m_matrix[3][2] = -m_matrix[3][0] * fA3 + m_matrix[3][1] * fA1 - m_matrix[3][2] * fA0;
	//	kInv.m_matrix[0][3] = -m_matrix[2][1] * fA5 + m_matrix[2][2] * fA4 - m_matrix[2][3] * fA3;
	//	kInv.m_matrix[1][2] = +m_matrix[2][0] * fA5 - m_matrix[2][2] * fA2 + m_matrix[2][3] * fA1;
	//	kInv.m_matrix[2][3] = -m_matrix[2][0] * fA4 + m_matrix[2][1] * fA2 - m_matrix[2][3] * fA0;
	//	kInv.m_matrix[3][3] = +m_matrix[2][0] * fA3 - m_matrix[2][1] * fA1 + m_matrix[2][2] * fA0;

	//	float fInvDet = ((float)1.0) / fDet;
	//	for (int iRow = 0; iRow < 4; iRow++)
	//	{
	//		for (int iCol = 0; iCol < 4; iCol++)
	//			kInv.m_matrix[iRow][iCol] *= fInvDet;
	//	}

	//	return(kInv);
	//}
}