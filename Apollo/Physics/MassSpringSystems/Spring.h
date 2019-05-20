#pragma once

#include "Math/Vector3.h"

NAME_SPACE_BEGIN_APOLLO


//弹簧粒子
struct SpringParticles
{
	Vector3			pos;	//位置
	Vector3			velocity;
	float					mass;
	unsigned int	adjacentParticles[4];		//二维gird，最多四个相邻节点。保存index
	int					adjacentParticlesCount;	
};

//保存粒子的数据结构
class SpringGridMesh
{	
public:
	friend class Spring;


private:
	int				m_width;
	int				m_height;
	std::vector<SpringParticles>	m_particlesList;
};

/*
*/
class Spring
{
public:
protected:
private:


};

NAME_SPACE_END
