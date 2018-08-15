#pragma once
#include "RenderPass.h"
namespace Apollo
{
	class RenderPass;
	class RenderState;

	class RenderPipeline
	{
	public:

		void				addRenderPass(RenderPassPtr renderPass) { m_renderPassList.push_back(renderPass); }

		virtual	void	render(RenderState* renderState);

	protected:

		std::vector<RenderPassPtr>			m_renderPassList;

	private:
	};

	typedef std::shared_ptr<RenderPipeline> RenderPipelinePtr;
}
