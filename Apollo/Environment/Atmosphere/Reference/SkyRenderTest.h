#pragma once

#include "Vector2.h"
#include "Vector3.h"
#include "Matrix4x4.h"
#include "Definitions.h"
#include "Environment/Atmosphere/Reference/Definitions.h"
#include "Texture3dDX11.h"

NAME_SPACE_BEGIN_APOLLO

class Camera;

NAME_SPACE_BEGIN_ATMOSPHERE
NAME_SPACE_BEGIN_REFERENCE

class SkyRenderCPUTest;

class SkyRenderTest
{
public:

	SkyRenderTest(int w, int h);

	void		renderSky();

	void		init();

	Texture3dDX11*		getScatteringTexture() { return m_scattering3DTexture; }

	Texture3dDX11*		getSingleMieScatteringTexture() { return m_singleMieScattering3DTexture; }

	const AtmosphereParameters&	 getAtmosphereParameters() { return m_atmosphereParameters; }

	//void		computeRayRadianceThread(const Vector2& uv, Vector3& outRadiance);
	void		onGUI();

protected:

	void			updateSunDirection();

	
	void			initLookupTexture();

	void			createScatteringTextureFromMemoryBuffer();

private:

	bool			m_isProcessing;
	int			m_windowWidth;
	int			m_windowHeight;

	Camera*	m_camera;

	SkyRenderCPUTest*		m_skyRenderCPUTest;

	float			m_sunTheta;	//[0,90] ¶È
	float			m_sunPhi;		//[0,360] ¶È
	Vector3	m_sunDirection;
	Vector3	m_earthSpacePosVec3;

	uint32_t	m_cpuSkyTextureHandle;

	std::vector<Vector3>		m_radianceRGBBuffer;

	std::vector<uint32_t>		m_rgba32Buffer;

	AtmosphereParameters m_atmosphereParameters;

	std::shared_ptr<ReducedScatteringTexture> m_scattering_texture;
	std::shared_ptr<ReducedScatteringTexture> m_single_mie_scattering_texture;

	Texture3dDX11*		m_scattering3DTexture;
	Texture3dDX11*		m_singleMieScattering3DTexture;
};

NAME_SPACE_END
NAME_SPACE_END
NAME_SPACE_END