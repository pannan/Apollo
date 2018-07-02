#pragma once

#include "Vector2f.h"
#include "Vector3f.h"
#include "Matrix4f.h"
#include "Matrix3f.h"

namespace Apollo
{
	class Camera
	{
	public:

		Camera();

		void onMouseMoved(int screenX,int screenY);

	protected:

		Vector3f projectOntoUnitSphere(Vector2f screenPos);

	private:

		float								m_pivotDistance;
		D3D11_VIEWPORT		m_viewPort;
	};
}
