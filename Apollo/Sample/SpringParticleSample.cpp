#include "stdafx.h"
#include "SpringParticleSample.h"


NAME_SPACE_BEGIN_APOLLO

SpringParticleSample::SpringParticleSample()
{

}

SpringParticleSample::~SpringParticleSample()
{

}

void SpringParticleSample::init()
{
	SpringParticles particle;

	//最简单的两个点的spring
	particle.force = 0;
	particle.mass = 1;
	particle.pos = Vector3(0, 10, 0);
	particle.velocity = Vector3::s_ZeroVec;
	particle.adjacentParticlesCount = 1;
	particle.adjacentParticles[0] = 1;
	m_springMesh.addParticles(particle);

	particle.pos = Vector3(0, 0, 0);
	particle.velocity = Vector3::s_ZeroVec;
	particle.adjacentParticlesCount = 1;
	particle.adjacentParticles[0] = 0;
	m_springMesh.addParticles(particle);
}

NAME_SPACE_END