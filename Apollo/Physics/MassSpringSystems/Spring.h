#pragma once

#include "Math/Vector3.h"

NAME_SPACE_BEGIN_APOLLO


//��������
struct SpringParticles
{
	Vector3			pos;	//λ��
	float				velocity;
	float				mass;
	size_t			adjacentParticles[4];		//��άgird������ĸ����ڽڵ㡣����index
	int				adjacentParticlesCount;	
};

//�������ӵ����ݽṹ
class SpringGridMesh
{	
public:
	friend class Spring;

	void				initTopology();

protected:

	void				getAdjacent(int x,int y,size_t  adjacent[4],int& adjacentCount);

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

	Spring(float springStiffness,float springLength,float dampingCoefficient);

	void		update(float dtime);

protected:

	Vector3		springForce(const Vector3& x0,const Vector3& x1);

	Vector3		dampingFroce(const Vector3& x0, const Vector3& x1, const float& v0, const float& v1);

private:

	float			m_springStiffness;
	float			m_springLength;
	float			m_dampingCoefficient;

};

NAME_SPACE_END
