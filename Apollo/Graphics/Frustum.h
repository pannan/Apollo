#pragma once

#include "Plane.h"
#include "Matrix4x4.h"

namespace Apollo
{
	class AABB;

	enum FrustumPlane
	{
		FP_LEFT,
		FP_RIGHT,
		FP_BOTTOM,
		FP_TOP,		
		FP_NEAR,
		FP_FAR
	};

	class Frustum
	{
	public:

		void				extractionPlane(const Matrix4x4& mat);

		bool				isVisible(const AABB& aabb);

	protected:

		Plane			m_frustumPlane[6];

	private:
	};
}
