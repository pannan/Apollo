/*******************************************************************
** �ļ���:	GeometryGenerator
** ��  Ȩ:
** ������:	�ܼ͸�
** ��  ��:	2019/5/22
** ��  ��:	1.0
** ��  ��:	����������Ķ���������
** Ӧ  ��:  ����Debug���ӻ�����

**************************** �޸ļ�¼ ******************************
** �޸���:
** ��  ��:
** ��  ��:
********************************************************************/

#pragma once

#include "Math/Vector3.h"

NAME_SPACE_BEGIN_APOLLO

class GeometryGenerator
{
public:

	//ԭ��Ĭ��Ϊ0���뾶Ϊ1.
	static void		generatorSphere(int phiStepCount,int thetaStepCount,std::vector<Vector3>& outPos,std::vector<unsigned short>& outIndex);

protected:

private:

	static	std::vector<Vector3>		m_vertexPool;
};

NAME_SPACE_END