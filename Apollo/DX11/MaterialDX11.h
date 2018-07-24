#pragma once

#include "ShaderDX11.h"

namespace Apollo
{
	class MaterialDX11
	{
	public:

		MaterialDX11(){}

		virtual ~MaterialDX11(){}


		ShaderDX11Ptr		m_vs;
		ShaderDX11Ptr		m_ps;

		std::string					m_albedoMap;
		std::string					m_normalMap;


	private:
	};

	typedef std::shared_ptr<MaterialDX11> MaterialDX11Ptr;
}
