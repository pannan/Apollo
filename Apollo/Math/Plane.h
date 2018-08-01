#pragma once

#include "Vector3.h"
#include "Vector4.h"

namespace Apollo
{
	class AABB;

	class Plane
	{
	public:

		float distance(const Vector3& pos)
		{
			return a * pos.m_x + b * pos.m_y + c * pos.m_z + d;
		}

		float distance(const Vector4& pos)
		{
			return a * pos.m_x + b * pos.m_y + c * pos.m_z + d * pos.m_w;
		}

		PlaneAABBIntersectType	intersectWithAABB(const AABB& aabb);

		float a;
		float b;
		float c;
		float d;

	protected:
	private:
	};
}