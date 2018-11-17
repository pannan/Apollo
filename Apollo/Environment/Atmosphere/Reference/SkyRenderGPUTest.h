#pragma once

#include "Vector2.h"
#include "Vector3.h"
#include "Matrix4x4.h"
//#include "Definitions.h"
#include "ShaderDX11.h"
#include "ModelDX11.h"

struct AtmosphereParameters;

NAME_SPACE_BEGIN_APOLLO

class Camera;
class Texture3dDX11;

NAME_SPACE_BEGIN_ATMOSPHERE
NAME_SPACE_BEGIN_REFERENCE

struct  GlobalParameters
{
	Vector4		eyeWorldSpacePosition;
	Vector4		eyeEarthSpacePosition;
	//float4x4	inverseViewMatrix;
	//float4x4	inverseProjMatrix;
	Matrix4x4 inverseViewProjMatrix;
	Matrix4x4	inverseViewMatrix;
	Vector4	projMat[4];
};

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

	void		initAtmosphereParameters(const Vector3& solarIrradiance, const Vector3& rayleighScattering, const Vector3& mieScattering,
		const Vector3& mieExtinction, const Vector3& groundAlbedo, const Vector3& absorptionExtinction);

	void		initShader();

	void		createQuadMesh();

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

	ShaderDX11Ptr		m_renderSkyVS;
	ShaderDX11Ptr		m_renderSkyPS;

	ConstantBufferDX11Ptr	m_globalParametersBuffer;
	ConstantBufferDX11Ptr	m_atmosphereParametersBuffer;

	ModelDX11Ptr m_quadModelPtr;
};

NAME_SPACE_END
NAME_SPACE_END
NAME_SPACE_END