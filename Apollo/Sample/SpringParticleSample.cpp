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

	m_renderState.setCullingMode(D3D11_CULL_NONE);
	m_renderState.setDepthWriteMask(D3D11_DEPTH_WRITE_MASK_ZERO);
	m_renderState.createState();

	m_camera = new Camera(Vector3(40, 30, 0), Vector3(0, 0, 0), Vector3(0, 1, 0), 0.001, 5000, 90 * _PI / 180.0f);
	m_camera->setViewportWidth(1280);
	m_camera->setViewportHeight(800);
}

NAME_SPACE_END