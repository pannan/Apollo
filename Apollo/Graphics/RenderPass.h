#pragma once

//#include "Visitor.h"
#include "Scene.h"

namespace Apollo
{
	class RenderState;
	class RenderPass// : public Visitor
	{
	public:

		void				setEnable(bool enable) { m_enable = enable; }

		bool				getEnable() { return m_enable; }

		virtual	void	preRender(RenderState& renderState);

		virtual	void render(RenderState& renderState);

		virtual	void postRender(RenderState& renderState);

		//virtual void visit(Scene& scene) {}
		//virtual void visit(SceneNode& node) {}
		//virtual void Visit(Mesh& mesh) = 0;

	protected:
	private:

		bool				m_enable;

		ScenePtr		m_scene;

	};
}
