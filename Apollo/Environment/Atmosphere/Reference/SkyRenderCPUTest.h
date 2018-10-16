#pragma once

#include "Vector2.h"
#include "Vector3.h"
#include "Matrix4x4.h"
#include "Definitions.h"

NAME_SPACE_BEGIN_APOLLO

class Camera;

NAME_SPACE_BEGIN_ATMOSPHERE
NAME_SPACE_BEGIN_REFERENCE

class SkyRenderCPUTest
{
public:

	SkyRenderCPUTest(int w, int h, Camera* camera);

	void		renderSingleScatting();

	void		init();

protected:


	Vector3 uvToCameraRay(Vector2 inUV, const Matrix4x4& projMat, const Matrix4x4& inverseViewMat);

	void		saveRadianceRGBBufferToFile();

private:

	int		m_windowWidth;
	int		m_windowHeight;

	Camera*	m_camera;

	std::vector<Vector3>		m_radianceRGBBuffer;

	AtmosphereParameters m_atmosphereParameters;
};

NAME_SPACE_END
NAME_SPACE_END
NAME_SPACE_END