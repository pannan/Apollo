#pragma once

namespace Apollo
{
	class Scene;
	class SceneNode;
	 
	class Visitor
	{
	public:

		virtual void visit(Scene& scene) = 0;
		virtual void visit(SceneNode& node) = 0;
		//virtual void Visit(Mesh& mesh) = 0;

	};
}
