#pragma once

#include "ShaderDX11.h"

namespace Apollo
{
	class ComputerShaderProcessTexture
	{
	public:

		void init();

		void render();

	protected:

		uint32_t			m_srcTextureHandle;
		uint32_t			m_uavTextureHandle;

		ShaderDX11Ptr		m_csShader;
		ShaderDX11Ptr		m_vsShader;
		ShaderDX11Ptr		m_psShader;

	private:
	};
}
