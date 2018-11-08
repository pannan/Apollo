#pragma once

#include "Vector2.h"
#include "Vector3.h"
#include "Matrix4x4.h"
#include "Definitions.h"

NAME_SPACE_BEGIN_APOLLO

class Camera;

NAME_SPACE_BEGIN_ATMOSPHERE
NAME_SPACE_BEGIN_REFERENCE

class SkyRenderGPUTest
{
public:

	SkyRenderGPUTest(int w, int h);

	//void		renderSingleScatting();

	void		init();

protected:


private:

	bool			m_isProcessing;
	int			m_windowWidth;
	int			m_windowHeight;

	Camera*	m_camera;

	float			m_sunTheta;	//[0,90] ��
	float			m_sunPhi;		//[0,360] ��
	Vector3	m_sunDirection;
	Vector3	m_earthSpacePosVec3;

	uint32_t	m_cpuSkyTextureHandle;

	std::vector<Vector3>		m_radianceRGBBuffer;

	std::vector<uint32_t>		m_rgba32Buffer;

	AtmosphereParameters* m_atmosphereParameters;
};

NAME_SPACE_END
NAME_SPACE_END
NAME_SPACE_END