#include "stdafx.h"
#include "SkyRenderGPUTest.h"
#include "Vector3.h"
#include "TextureDX11ResourceFactory.h"


typedef Apollo::Vector3 float3 ;
#include "../bin/Assets/Shader/SkyDefinitions.hlsl"

NAME_SPACE_BEGIN_APOLLO
NAME_SPACE_BEGIN_ATMOSPHERE
NAME_SPACE_BEGIN_REFERENCE

SkyRenderGPUTest::SkyRenderGPUTest(int w, int h) : m_windowWidth(w), m_windowHeight(h)
{
	m_atmosphereParameters = new AtmosphereParameters;
}

SkyRenderGPUTest::~SkyRenderGPUTest()
{
	SAFE_DELETE(m_atmosphereParameters);
}

void SkyRenderGPUTest::init(const Vector3& solarIrradiance, const Vector3& rayleighScattering,const Vector3& mieScattering,
	const Vector3& mieExtinction,const Vector3& groundAlbedo,const Vector3& absorptionExtinction)
{
	initAtmosphereParameters(solarIrradiance, rayleighScattering, mieScattering, mieExtinction, groundAlbedo, absorptionExtinction);

	initShader();

	createQuadMesh();

	//create rtt
	Texture2dConfigDX11 tex2dConfig;
	tex2dConfig.SetWidth(m_windowWidth);
	tex2dConfig.SetHeight(m_windowHeight);
	tex2dConfig.SetBindFlags(D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET);
	TextureDX11ResourceFactory::getInstance().createTexture2D("SkyRenderRTT", tex2dConfig);
}

void SkyRenderGPUTest::initAtmosphereParameters(const Vector3& solarIrradiance, const Vector3& rayleighScattering, const Vector3& mieScattering,
	const Vector3& mieExtinction, const Vector3& groundAlbedo, const Vector3& absorptionExtinction)
{
	constexpr Length kRayleighScaleHeight = 8000.0;
	constexpr Length kMieScaleHeight = 1200.0;

	m_atmosphereParameters->solar_irradiance = solarIrradiance;
	m_atmosphereParameters->sun_angular_radius = 0.2678 * deg;
	m_atmosphereParameters->bottom_radius = 6360.0 * km;
	m_atmosphereParameters->top_radius = 6420.0 * km;
	//m_atmosphereParameters->rayleigh_density.layers[1] = DensityProfileLayer(0.0 * m, 1.0, -1.0 / kRayleighScaleHeight, 0.0 / m, 0.0);
	m_atmosphereParameters->rayleigh_density.layers[1].width = 0.0 * km;
	m_atmosphereParameters->rayleigh_density.layers[1].exp_term = 1.0f;
	m_atmosphereParameters->rayleigh_density.layers[1].exp_scale = -1.0 / kRayleighScaleHeight;
	m_atmosphereParameters->rayleigh_density.layers[1].linear_term = 0.0;
	m_atmosphereParameters->rayleigh_density.layers[1].constant_term = 0.0;
	m_atmosphereParameters->rayleigh_scattering = rayleighScattering;
	//m_atmosphereParameters->mie_density.layers[1] = DensityProfileLayer(0.0 * m, 1.0, -1.0 / kMieScaleHeight, 0.0 / m, 0.0);
	m_atmosphereParameters->mie_density.layers[1].width = 0.0 * km;
	m_atmosphereParameters->mie_density.layers[1].exp_term = 1.0f;
	m_atmosphereParameters->mie_density.layers[1].exp_scale = -1.0 / kMieScaleHeight;
	m_atmosphereParameters->mie_density.layers[1].linear_term = 0.0;
	m_atmosphereParameters->mie_density.layers[1].constant_term = 0.0;

	m_atmosphereParameters->mie_scattering = mieScattering;
	m_atmosphereParameters->mie_extinction = mieExtinction;
	m_atmosphereParameters->mie_phase_function_g = 0.8;
	//	m_atmosphereParameters->absorption_density.layers[0] = DensityProfileLayer(25.0 * km, 0.0, 0.0 / km, 1.0 / (15.0 * km), -2.0 / 3.0);
	m_atmosphereParameters->absorption_density.layers[0].width = 25.0 * km;
	m_atmosphereParameters->absorption_density.layers[0].exp_term = 0.0f;
	m_atmosphereParameters->absorption_density.layers[0].exp_scale = 0.0f;
	m_atmosphereParameters->absorption_density.layers[0].linear_term = 1.0 / (15.0 * km);
	m_atmosphereParameters->absorption_density.layers[0].constant_term = -2.0 / 3.0;
	//m_atmosphereParameters->absorption_density.layers[1] = DensityProfileLayer(0.0 * km, 0.0, 0.0 / km, -1.0 / (15.0 * km), 8.0 / 3.0);
	m_atmosphereParameters->absorption_density.layers[1].width = 0.0 * km;
	m_atmosphereParameters->absorption_density.layers[1].exp_term = 0.0f;
	m_atmosphereParameters->absorption_density.layers[1].exp_scale = 0.0f;
	m_atmosphereParameters->absorption_density.layers[1].linear_term = -1.0 / (15.0 * km);
	m_atmosphereParameters->absorption_density.layers[1].constant_term = 8.0 / 3.0;
	m_atmosphereParameters->absorption_extinction = absorptionExtinction;
	m_atmosphereParameters->ground_albedo = groundAlbedo;
	m_atmosphereParameters->mu_s_min = cos(102.0 * deg);
}

void SkyRenderGPUTest::initShader()
{
	//创建shader
	m_renderSkyVS = ShaderDX11Ptr(new ShaderDX11());
	m_renderSkyVS->loadShaderFromFile(ShaderType::VertexShader,
		"../bin/Assets/Shader/MySkyTest.hlsl",
		ShaderMacros(),
		"VSMAIN",
		"vs_5_0");

	m_renderSkyPS = ShaderDX11Ptr(new ShaderDX11());
	m_renderSkyPS->loadShaderFromFile(ShaderType::PixelShader,
		"../bin/Assets/Shader/MySkyTest.hlsl",
		ShaderMacros(),
		"PSMAIN",
		"ps_5_0");

	//创建buffer
	m_globalParametersBuffer = ConstantBufferDX11Ptr(new ConstantBufferDX11(sizeof(GlobalParameters), true, true, nullptr));
	m_renderSkyVS->setConstantBuffer("GlobalParameters", m_globalParametersBuffer);
	m_renderSkyPS->setConstantBuffer("GlobalParameters", m_globalParametersBuffer);

	D3D11_SUBRESOURCE_DATA atmosphereSubResourceData;
	atmosphereSubResourceData.pSysMem = m_atmosphereParameters;
	atmosphereSubResourceData.SysMemPitch = 0;
	atmosphereSubResourceData.SysMemSlicePitch = 0;
	m_atmosphereParametersBuffer = ConstantBufferDX11Ptr(new ConstantBufferDX11(sizeof(AtmosphereParameters), true, 
		true, &atmosphereSubResourceData));

	m_renderSkyVS->setConstantBuffer("AtmosphereParameters", m_atmosphereParametersBuffer);
	m_renderSkyPS->setConstantBuffer("AtmosphereParameters", m_atmosphereParametersBuffer);
}

void SkyRenderGPUTest::createQuadMesh()
{
	m_quadModelPtr = ModelDX11::createFullScreenQuadModel();

	MaterialDX11* material = new MaterialDX11;
	material->m_vs = m_renderSkyVS;
	material->m_ps[(uint8_t)RenderPipelineType::ForwardRender] = m_renderSkyPS;
	m_quadModelPtr->addMaterial(MaterialPtr(material));
}

NAME_SPACE_END
NAME_SPACE_END
NAME_SPACE_END