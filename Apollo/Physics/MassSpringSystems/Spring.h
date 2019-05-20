#pragma once

#include "Math/Vector3.h"

NAME_SPACE_BEGIN_APOLLO


//��������
struct SpringParticles
{
	Vector3			pos;	//λ��
	Vector3			velocity;
	float					mass;
	unsigned int	adjacentParticles[4];		//��άgird������ĸ����ڽڵ㡣����index
	int					adjacentParticlesCount;	
};

//�������ӵ����ݽṹ
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
