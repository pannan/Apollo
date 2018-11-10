#pragma once

#include "Vector2.h"
#include "Vector3.h"
#include "Matrix4x4.h"
//#include "Definitions.h"

struct AtmosphereParameters;

NAME_SPACE_BEGIN_APOLLO

class Camera;
class Texture3dDX11;

NAME_SPACE_BEGIN_ATMOSPHERE
NAME_SPACE_BEGIN_REFERENCE

class SkyRenderGPUTest
{
public:

	SkyRenderGPUTest(int w, int h);
	~SkyRenderGPUTest();

	//void		renderSingleScatting();

	void		init(const Vector3& solarIrradiance,const Vector3& rayleighScattering,const Vector3& mieScattering,const Vector3& mieExtinction,
		const Vector3& groundAlbedo,const Vector3& absorptionExtinction);

	void		renderSky();

protected:


private:

	bool			m_isProcessing;
	int			m_windowWidth;
	int			m_windowHeight;

	Camera*	m_camera;

	float			m_sunTheta;	//[0,90] ¶È
	float			m_sunPhi;		//[0,360] ¶È
	Vector3	m_sunDirection;
	Vector3	m_earthSpacePosVec3;

	uint32_t	m_cpuSkyTextureHandle;

	std::vector<Vector3>		m_radianceRGBBuffer;

	std::vector<uint32_t>		m_rgba32Buffer;

	AtmosphereParameters* m_atmosphereParameters;

	Texture3dDX11*		m_scattering3DTexture;
	Texture3dDX11*		m_singleMieScattering3DTexture;
};

NAME_SPACE_END
NAME_SPACE_END
NAME_SPACE_END