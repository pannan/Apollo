#pragma once

#include "Math/Vector3.h"

NAME_SPACE_BEGIN_APOLLO


//弹簧粒子
struct SpringParticles
{
	Vector3		pos;	//位置
	Vector3		velocity;
	float				mass;
	size_t			adjacentParticles[4];		//二维gird，最多四个相邻节点。保存index
	int				adjacentParticlesCount;	

	Vector3		force;
};

//保存粒子的数据结构
class SpringGridMesh
{	
public:
	friend class Spring;

	void				initTopology();

	void				addParticles(SpringParticles& particle) { m_particlesList.push_back(particle); }

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

	void		update(SpringGridMesh& springMesh,float dtime);

protected:

	void				updateForce(SpringGridMesh& springMesh,float dtime);

	void				verletIntegrationUpdate(SpringGridMesh& springMesh, float dtime);

	Vector3		springForce(const Vector3& x0,const Vector3& x1);

	Vector3		dampingFroce(const Vector3& x0, const Vector3& x1, const Vector3& v0, const Vector3& v1);

	Vector3		computeForce(const Vector3& x0, const Vector3& x1, const Vector3& v0, const Vector3& v1);

private:

	float			m_springStiffness;
	float			m_springLength;
	float			m_dampingCoefficient;

};

NAME_SPACE_END
