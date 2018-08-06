#pragma once

#include "MaterialDX11.h"

namespace Apollo
{
	class Visitor;
	class SceneNode;
	class IRenderable;
	
	class Scene
	{
	public:

		Scene();

		virtual ~Scene();

		virtual	void		update();

		virtual	void		render();

		virtual	void		addRenderable(IRenderable* renderable);

		//virtual void		accept(Visitor* visitor);

	protected:

		std::vector<IRenderable*>			m_visibleRenderableList;

		std::vector<MaterialDX11Ptr>		m_materialList;

		uint16_t										m_currentRenderMaterial;

	private:
	};

	typedef std::shared_ptr<Scene>	ScenePtr;
}