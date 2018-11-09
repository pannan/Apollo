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

class SkyRenderCPUTest
{
public:

	SkyRenderCPUTest(int w, int h);

	void		renderSky(Camera* camera,Vector3 sunDirection);

	void		init(std::shared_ptr<ReducedScatteringTexture>& scatteringTexture,
		std::shared_ptr<ReducedScatteringTexture>& singleMieScatteringTexture);

	void		saveRadianceRGBBufferToFile();

	const AtmosphereParameters&	 getAtmosphereParameters() { return m_atmosphereParameters; }

	//void		computeRayRadianceThread(const Vector2& uv, Vector3& outRadiance);
	//void		onGUI();

	uint32_t	getSkyRenderTextureHandle() { return m_cpuSkyTextureHandle; }

	//把光谱相关参数转换成rgb
	Vector3		getVec3SolarIrradiance();

	Vector3		getVec3MieScattering();

	Vector3		getVec3MieExtinction();

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

	//void			updateSunDirection();

	//void			checkRMuMusNuConversion();

	void			initLookupTexture();

	//void			createScatteringTextureFromMemoryBuffer();

private:

	bool			m_isProcessing;
	int			m_windowWidth;
	int			m_windowHeight;

//	Camera*	m_camera;

	//float			m_sunTheta;	//[0,90] 度
//	float			m_sunPhi;		//[0,360] 度
//	Vector3	m_sunDirection;
	Vector3	m_earthSpacePosVec3;

	uint32_t	m_cpuSkyTextureHandle;

	std::vector<Vector3>		m_radianceRGBBuffer;

	std::vector<uint32_t>		m_rgba32Buffer;

	AtmosphereParameters m_atmosphereParameters;

	std::shared_ptr<ReducedScatteringTexture> m_scattering_texture;
	std::shared_ptr<ReducedScatteringTexture> m_single_mie_scattering_texture;
};

NAME_SPACE_END
NAME_SPACE_END
NAME_SPACE_END