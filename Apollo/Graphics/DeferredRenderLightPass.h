#pragma once

#include "RenderPass.h"
#include "LightDefine.h"

namespace Apollo
{
	class DeferredRenderLightPass : public RenderPass
	{
	public:

		DeferredRenderLightPass();

		DeferredRenderLightPass(ScenePtr& scene, RenderState* renderState);

		void				setEnable(bool enable) { m_enable = enable; }

		bool				getEnable() { return m_enable; }

		virtual	void	preRender(RenderState* renderState);

		virtual	void render(RenderState* renderState);

		virtual	void postRender(RenderState* renderState);

		void				clearLight() { m_lightList.clear(); }

		void				addLight(const LightPtr& light) { m_lightList.push_back(light); }

	protected:

		std::vector<LightPtr>		m_lightList;

	private:
	};
}
