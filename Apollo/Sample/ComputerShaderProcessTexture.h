#pragma once

#include "ShaderDX11.h"

namespace Apollo
{
	class ComputerShaderProcessTexture
	{
	public:

		void init();

	protected:

		uint32_t			m_textureHandle;

		ShaderDX11Ptr		m_csShader;
		ShaderDX11Ptr		m_vsShader;
		ShaderDX11Ptr		m_psShader;

	private:
	};
}
