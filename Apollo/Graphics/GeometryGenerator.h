/*******************************************************************
** 文件名:	GeometryGenerator
** 版  权:
** 创建人:	熊纪刚
** 日  期:	2019/5/22
** 版  本:	1.0
** 描  述:	参数几何体的顶点数据与
** 应  用:  用作Debug可视化需求

**************************** 修改记录 ******************************
** 修改人:
** 日  期:
** 描  述:
********************************************************************/

#pragma once

#include "Math/Vector3.h"

NAME_SPACE_BEGIN_APOLLO

class GeometryGenerator
{
public:

	//原点默认为0，半径为1.
	static void		generatorSphere(int phiStepCount,int thetaStepCount,std::vector<Vector3>& outPos,std::vector<unsigned short>& outIndex);

protected:

private:

	static	std::vector<Vector3>		m_vertexPool;
};

NAME_SPACE_END