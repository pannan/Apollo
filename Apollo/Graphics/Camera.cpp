#include "stdafx.h"
#include "Camera.h"
#include "math.h"
using namespace Apollo;

Camera::Camera()
{
	m_pivotDistance = 5;
}

Vector3f Camera::projectOntoUnitSphere(Vector2f screenPos)
{
	// Map the screen coordinates so that (0, 0) is the center of the viewport.
	screenPos -= Vector2f(m_viewPort.Width,m_viewPort.Height) * 0.5f;

	float x, y, z;
	// The radius of the unit sphere is 1/2 of the shortest dimension of the viewport.
	float radius = _min(m_viewPort.Width, m_viewPort.Height) * 0.5f;

	// Now normalize the screen coordinates into the range [-1 .. 1].
	x = screenPos.x / radius;
	// The y-coordinate has to be inverted so that +1 is the top of the unit sphere
	// and -1 is the bottom of the unit sphere.
	y = -(screenPos.y / radius);

	float length_sqr = (x * x) + (y * y);

	// If the screen point is mapped outside of the unit sphere
	if (length_sqr > 1.0f)
	{
		float invLength = 1.0f / sqrt(length_sqr);

		// Return the normalized point that is closest to the outside of the unit sphere.
		x *= invLength;
		y *= invLength;
		z = 0.0f;
	}
	else
	{
		// The point is on the inside of the unit sphere.
		z = sqrt(1.0f - length_sqr);
		// If we are "inside" the unit sphere, then 
		// invert the z component.
		// In a right-handed coordinate system, the "+z" axis of the unit
		// sphere points towards the viewer. If we are in the Unit sphere, we
		// want to project the point to the inside of the sphere and in this case
		// the z-axis we want to project on points away from the viewer (-z).
		if (m_pivotDistance <= 0.0f)
		{
			z = -z;
		}
	}

	return Vector3f(x, y, z);
}

void Camera::onMouseMoved(int screenX, int screenY)
{
	Vector3f currentPoint = projectOntoUnitSphere(Vector2f(screenX,screenY));

	AddRotation(glm::quat(currentPoint, m_PreviousPoint));

	m_PreviousPoint = currentPoint;
}