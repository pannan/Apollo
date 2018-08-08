#pragma once

#include "Material.h"
#include "ShaderDX11.h"

namespace Apollo
{
	class MaterialDX11 : public Material
	{
	public:

		MaterialDX11(){}

		virtual ~MaterialDX11(){}

		virtual	void		bind() const;

		virtual	void		unBind() const;


		ShaderDX11Ptr		m_vs;
		ShaderDX11Ptr		m_ps;

	private:
	};

	typedef std::shared_ptr<MaterialDX11> MaterialDX11Ptr;
}
