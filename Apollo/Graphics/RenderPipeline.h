#pragma once

namespace Apollo
{
	class RenderPass;
	class RenderState;

	class RenderPipeline
	{
	public:

		void				addRenderPass(RenderPass* renderPass) { m_renderPassList.push_back(renderPass); }

		virtual	void	render(RenderState* renderState);

	protected:

		std::vector<RenderPass*>			m_renderPassList;

	private:
	};

	typedef std::shared_ptr<RenderPipeline> RenderPipelinePtr;
}
