#pragma once

#include "ShaderDX11.h"
#include "MeshDX11.h"
#include "RenderStateDX11.h"
namespace Apollo
{
	class ComputerShaderProcessTexture
	{
	public:

		ComputerShaderProcessTexture();
		~ComputerShaderProcessTexture();

		void init();

		void render();

	protected:

		void	initQuadMesh();

	protected:

		uint32_t			m_srcTextureHandle;
		uint32_t			m_uavTextureHandle;

		ShaderDX11Ptr		m_csShader;
		ShaderDX11Ptr		m_vsShader;
		ShaderDX11Ptr		m_psShader;

		MeshDX11Ptr		m_quadMesh;

		RenderStateDX11 m_renderState;

	private:
	};
}
