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

Vector3 Spring::springForce(const Vector3& x0, const Vector3& x1)
{
	Vector3 vec = x1 - x0;
	float vecLength = vec.length();
	vec.normalize();
	Vector3 force = vec * m_springStiffness  * (vecLength - m_springLength);

	return force;
}

Vector3 Spring::dampingFroce(const Vector3& x0, const Vector3& x1, const float& v0, const float& v1)
{
	float velocity = v1 - v0;
	Vector3 vec = x1 - x0;
	vec.normalize();

	Vector3 force = vec * velocity * m_dampingCoefficient;
	return force;
}

void Spring::update(float dtime)
{

}

NAME_SPACE_END