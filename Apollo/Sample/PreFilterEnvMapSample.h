#pragma once

#include "SampleBase.h"
#include "ShaderDX11.h"
#include "ModelDX11.h"
#include "RenderStateDX11.h"

NAME_SPACE_BEGIN_APOLLO

class PreFilterEnvMapSample : public SampleBase
{
public:

	PreFilterEnvMapSample();
	~PreFilterEnvMapSample();

	virtual void  init();

	virtual void	render() {}

	virtual Camera* getCamera() { return nullptr; }

protected:

	void initQuadMesh();

protected:

	uint32_t			m_srcTextureHandle;

	//ShaderDX11Ptr		m_csShader;
	ShaderDX11Ptr		m_vsShader;
	ShaderDX11Ptr		m_psShader;

	ModelDX11				m_quadModel;

	RenderStateDX11 m_renderState;

protected:

private:
};

NAME_SPACE_END
