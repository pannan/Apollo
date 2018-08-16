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

		//vs对forward和deferred渲染是一样的
		ShaderDX11Ptr		m_vs;
		ShaderDX11Ptr		m_ps[(uint8_t)RenderPipelineType::RenderPipelineCount];

		static RenderPipelineType s_currentRenderPipeline;

	private:
	};

	typedef std::shared_ptr<MaterialDX11> MaterialDX11Ptr;
}
