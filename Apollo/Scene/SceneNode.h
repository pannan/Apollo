#pragma once

#include "Matrix4x4.h"
#include "AABB.h"

namespace Apollo
{
	class SceneNode
	{
	public:

		SceneNode();

		virtual ~SceneNode();



	protected:

		Matrix4x4		m_worldMat;
		AABB				m_woldAABB;

	private:
	};
}
