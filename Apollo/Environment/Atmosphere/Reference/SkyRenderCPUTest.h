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

	SkyRenderCPUTest(int w, int h);

	void		renderSingleScatting();

	void		init();

	void		saveRadianceRGBBufferToFile();

	//void		computeRayRadianceThread(const Vector2& uv, Vector3& outRadiance);
	void		onGUI();

protected:

	Vector2	normalizeUV(float x, float y)
	{
		float u = (float)x / m_windowWidth;
		float v = (float)y / m_windowHeight;
		return Vector2(u, v);
	}

	void			updateCpuSkyTexture();

	void			rgbaFloatBufferToRgba32Buffer();

	//Vector3 uvToCameraRay(Vector2 inUV, const Matrix4x4& projMat, const Matrix4x4& inverseViewMat);

	void			updateSunDirection();

	void			checkRMuMusNuConversion();

private:

	bool			m_isProcessing;
	int		m_windowWidth;
	int		m_windowHeight;

	Camera*	m_camera;

	float			m_sunTheta;	//[0,90] ¶È
	float			m_sunPhi;		//[0,360] ¶È
	Vector3	m_sunDirection;
	Vector3	m_earthSpacePosVec3;

	uint32_t	m_cpuSkyTextureHandle;

	std::vector<Vector3>		m_radianceRGBBuffer;

	std::vector<uint32_t>		m_rgba32Buffer;

	AtmosphereParameters m_atmosphereParameters;
};

NAME_SPACE_END
NAME_SPACE_END
NAME_SPACE_END