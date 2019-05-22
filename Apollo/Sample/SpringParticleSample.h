#pragma once

#include "SampleBase.h"
#include "ConstantBufferDX11.h"
#include "ModelDX11.h"
#include "ShaderDX11.h"
#include "RenderStateDX11.h"
#include "Physics/MassSpringSystems/Spring.h"

NAME_SPACE_BEGIN_APOLLO

class SpringParticleSample : public SampleBase
{
public:
	SpringParticleSample();
	~SpringParticleSample();

	virtual void  init();

	virtual void	render();

	virtual void onGUI();

	virtual Camera* getCamera() { return m_camera; }

protected:



private:

	Camera * m_camera;
	RenderStateDX11		m_renderState;

	SpringGridMesh	m_springMesh;
	Spring					m_spring;
};

NAME_SPACE_END
