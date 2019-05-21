#include "stdafx.h"
#include "Spring.h"
#include "LogManager.h"

NAME_SPACE_BEGIN_APOLLO

void SpringGridMesh::getAdjacent(int x, int y, size_t adjacent[4], int& adjacentCount)
{
	adjacentCount = 0;

	//x大于0，有(x - 1,y)的相邻粒子
	if (x > 0)
	{
		adjacent[adjacentCount] = y * m_width + x - 1;
		++adjacentCount;		
	}

	//x小于< m_width - 1，有(x + 1,y)的相邻粒子
	if (x < m_width - 1)
	{
		adjacent[adjacentCount] = y * m_width + x + 1;
		++adjacentCount;
	}

	//y大于0，有(x,y - 1)的相邻粒子
	if (y > 0)
	{
		adjacent[adjacentCount] = (y - 1) * m_width + x;
		++adjacentCount;
	}

	//y小于< m_height - 1，有(x,y + 1)的相邻粒子
	if (y < m_width - 1)
	{
		adjacent[adjacentCount] = (y +1) * m_width + x;
		++adjacentCount;
	}
}

void SpringGridMesh::initTopology()
{
	if (m_particlesList.size() != m_width * m_height)
	{
		LogManager::getInstance().log("m_particlesList.size() != m_width * m_height");
		return;
	}

	for (int y = 0; y < m_height; ++y)
	{
		for (int x = 0; x < m_width; ++x)
		{
			size_t index = y * m_width + x;

			int adjacentCount = 0;
			size_t adjacent[4];
			getAdjacent(x,y,adjacent,adjacentCount);
			
			memcpy(m_particlesList[index].adjacentParticles, adjacent, sizeof(size_t) * 4);
			m_particlesList[index].adjacentParticlesCount = adjacentCount;
		}
	}
}

//////////////////////////////////////////////////////////////////////////

Spring::Spring(float springStiffness,float springLength,float dampingCoefficient) : m_springLength(springLength) , 
																						m_springStiffness(springStiffness),
																						m_dampingCoefficient(dampingCoefficient)
{

}

/*
springforce = Ks * (x1 - x0) / length(x1 - x0) * (lenght(x1 - x0) - springLength)
*/

Vector3 Spring::springForce(const Vector3& x0, const Vector3& x1)
{
	Vector3 vec = x1 - x0;
	float vecLength = vec.length();
	vec.normalize();
	Vector3 force = vec * m_springStiffness  * (vecLength - m_springLength);

	return force;
}

/*
dampingforce = Kd * (v1 - v0) dot ((x1 - x0) / length(x1 - x0))
这里有问题，按上面dot出来的是一个float，但是springforce的是float3
从代码上看着里少乘一个normalize(x1 - x0)
修改后的公式:
normalize(x1 - x0) = (x1 - x0) / length(x1 - x0)
dampingforce = Ks * dot( (v1 - v0), normalize(x1 - x0)) * normalize(x1 - x0);
*/

Vector3 Spring::dampingFroce(const Vector3& x0, const Vector3& x1, const Vector3& v0, const Vector3& v1)
{
	Vector3 velocity = v1 - v0;
	Vector3 vec = x1 - x0;
	vec.normalize();

	Vector3 force = vec * vec.dot(velocity) * m_dampingCoefficient;
	return force;
}

Vector3 Spring::computeForce(const Vector3& x0, const Vector3& x1, const Vector3& v0, const Vector3& v1)
{
	Vector3 force0 = springForce(x0, x1);
	Vector3 force1 = dampingFroce(x0, x1, v0, v1);
	return force0 + force1;
}

void Spring::updateForce(SpringGridMesh& springMesh,float dtime)
{
	for (size_t i = 0; i < springMesh.m_particlesList.size(); ++i)
	{
		Vector3 particelsForce = Vector3::s_ZeroVec;

		SpringParticles& particle = springMesh.m_particlesList[i];

		for (int j = 0; j < particle.adjacentParticlesCount; ++j)
		{
			size_t adjacentIndex = particle.adjacentParticles[j];
			SpringParticles& adjacentParticle = springMesh.m_particlesList[adjacentIndex];

			Vector3 force = computeForce(particle.pos, adjacentParticle.pos, particle.velocity, adjacentParticle.velocity);
			particelsForce += force;
		}

		//加上重力和碰撞力，这里碰撞力忽略
		particelsForce += Vector3(0, -1, 0) * 9.8f * particle.mass;

		particle.force = particelsForce;
	}
}

void Spring::verletIntegrationUpdate(SpringGridMesh& springMesh, float dtime)
{
	for (size_t i = 0; i < springMesh.m_particlesList.size(); ++i)
	{
		SpringParticles& particle = springMesh.m_particlesList[i];

		Vector3 nextPos = particle.pos + particle.velocity * dtime + particle.force * dtime * dtime / particle.mass;
		particle.velocity = (nextPos - particle.pos) / dtime;
		particle.pos = nextPos;
	}
}

void Spring::update(SpringGridMesh& springMesh,float dtime)
{
	updateForce(springMesh, dtime);

	verletIntegrationUpdate(springMesh, dtime);
}

NAME_SPACE_END