#pragma once

#include "RenderPass.h"
#include "Matrix4x4.h"

namespace Apollo
{
	class BasePass : public RenderPass
	{
	public:

		virtual void preRender(RenderState& e);
		virtual void render(RenderState& e);
		virtual void postRender(RenderState& e);

		virtual void visit(Scene& scene);
		virtual void visit(SceneNode& node);
		//virtual void visit(Mesh& mesh);

	protected:

		/*	__declspec(align(16)) struct PerObject
			{
				Matrix4x4 worldView;
				Matrix4x4 modelViewProjection;
			};*/

	private:
	};
		
}
