#pragma once

#include "Visitor.h"

namespace Apollo
{
	class RenderPass : public Visitor
	{
	public:

		void				setEnable(bool enable) { m_enable = enable; }

		bool				getEnable() { return m_enable; }

		virtual	void	preRender() = 0;

		virtual	void render() = 0;

		virtual	void postRender() = 0;

		virtual void visit(Scene& scene) = 0;
		virtual void visit(SceneNode& node) = 0;
		//virtual void Visit(Mesh& mesh) = 0;

	protected:
	private:

		bool				m_enable;
	};
}
