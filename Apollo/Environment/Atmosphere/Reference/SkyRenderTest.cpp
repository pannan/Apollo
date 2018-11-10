#include "stdafx.h"
#include "SkyRenderTest.h"
#include "Camera.h"
#include "Environment/Atmosphere/Reference/Functions.h"
//#include "Environment/Atmosphere/Reference/Definitions.h"
#include "Environment/Atmosphere/Constants.h"
#include "Texture2dConfigDX11.h"
#include "TextureDX11ResourceFactory.h"
#include "RendererDX11.h"
#include "Texture2dDX11.h"
//#include "SDK/minpng.h"
#include <thread>
#include "imgui.h"
#include<atomic>  
#include "TextureDX11ResourceFactory.h"
#include "Texture3dConfigDX11.h"
#include "Environment/Atmosphere/Reference/SkyRenderCPUTest.h"
#include "Environment/Atmosphere/Reference/SkyRenderGPUTest.h"

//#define _IN(x) const x&
//#define _OUT(x) x&
//#define TEMPLATE(x) template<class x>
//#define TEMPLATE_ARGUMENT(x) <x>



NAME_SPACE_BEGIN_APOLLO
NAME_SPACE_BEGIN_ATMOSPHERE
NAME_SPACE_BEGIN_REFERENCE

typedef dimensional::vec2	float2;
typedef dimensional::vec3	float3;
typedef dimensional::vec4	float4;

using std::max;
using std::min;

constexpr Wavelength kLambdaR = 680.0 * nm;
constexpr Wavelength kLambdaG = 550.0 * nm;
constexpr Wavelength kLambdaB = 440.0 * nm;

SkyRenderTest::SkyRenderTest(int w, int h)
{
	m_isProcessing = false;
	m_windowWidth = w;
	m_windowHeight = h;

	m_sunTheta = 85.0f;
	m_sunPhi = 0.0f;

	m_cpuSkyTextureHandle = 0;

	Vector3 camPos(0, 1000, 0);
	Vector3 lookAt = camPos + Vector3(3, 8, 1) * 10;
	m_camera = new Camera(camPos, lookAt, Vector3(0, 1, 0), 0.001, 5000, 90 * _PI / 180.0f);
	m_camera->setViewportWidth(w);
	m_camera->setViewportHeight(h);
	m_camera->updateViewProjMatrix();

	m_skyRenderCPUTest = new SkyRenderCPUTest(w,h);
	m_skyRenderGPUTest = new SkyRenderGPUTest(w, h);
}

SkyRenderTest::~SkyRenderTest()
{
	SAFE_DELETE(m_skyRenderCPUTest);
	SAFE_DELETE(m_skyRenderGPUTest);
}

void SkyRenderTest::init()
{
	initLookupTexture();
	m_skyRenderCPUTest->init(m_scattering_texture, m_single_mie_scattering_texture);

	Vector3 absorptionExtinction = m_skyRenderCPUTest->getVec3AbsorptionExtinction();
	Vector3 groundAlbedo = m_skyRenderCPUTest->getVec3GroundAlbedo();
	Vector3 mieExtinction = m_skyRenderCPUTest->getVec3MieExtinction();
	Vector3 mieScattering = m_skyRenderCPUTest->getVec3MieScattering();
	Vector3 solarIrradiance = m_skyRenderCPUTest->getVec3SolarIrradiance();
	Vector3 rayleighScattering = m_skyRenderCPUTest->getRayleighScattering();

	m_skyRenderGPUTest->init(solarIrradiance, rayleighScattering, mieScattering, mieExtinction, groundAlbedo, absorptionExtinction);
}

void SkyRenderTest::initLookupTexture()
{
	//transmittance_texture_.reset(new TransmittanceTexture());
	m_scattering_texture.reset(new ReducedScatteringTexture());
	m_single_mie_scattering_texture.reset(new ReducedScatteringTexture());

	const size_t testSize = sizeof(IrradianceSpectrum);
	std::ifstream file;
	file.open("h:\\scattering.dat");
	if (!file.good())
		return;


	file.close();
	//transmittance_texture_->Load(cache_directory_ + "transmittance.dat");
	m_scattering_texture->Load("h:\\scattering.dat");
	m_single_mie_scattering_texture->Load("h:\\single_mie_scattering.dat");
	//irradiance_texture_->Load(cache_directory_ + "irradiance.dat");


	//createScatteringTextureFromMemoryBuffer();
}

void SkyRenderTest::createScatteringTextureFromMemoryBuffer()
{
	//scattering texture
	std::vector<Vector4> rgbScatteringFloatBuffer;
	rgbScatteringFloatBuffer.resize(SCATTERING_TEXTURE_WIDTH * SCATTERING_TEXTURE_HEIGHT * SCATTERING_TEXTURE_DEPTH);

	for (int k = 0; k < SCATTERING_TEXTURE_DEPTH; ++k)
	{
		float fk = (float)(k + 0.5) / SCATTERING_TEXTURE_DEPTH;

		for (int j = 0; j < SCATTERING_TEXTURE_HEIGHT; ++j)
		{
			float fj = (float)(j + 0.5) / SCATTERING_TEXTURE_HEIGHT;

			for (int i = 0; i < SCATTERING_TEXTURE_WIDTH; ++i)
			{
				float fi = (float)(i + 0.5) / SCATTERING_TEXTURE_WIDTH;

				float3 uvw(fi, fj, fk);
				IrradianceSpectrum irradiacne = lookupScatteringTexture(*m_scattering_texture.get(), uvw);
				float r = irradiacne(kLambdaR).to(watt_per_square_meter_per_nm);
				float g = irradiacne(kLambdaG).to(watt_per_square_meter_per_nm);
				float b = irradiacne(kLambdaB).to(watt_per_square_meter_per_nm);

				size_t index = k * (SCATTERING_TEXTURE_HEIGHT * SCATTERING_TEXTURE_WIDTH) +
					j * SCATTERING_TEXTURE_WIDTH + i;
				rgbScatteringFloatBuffer[index] = Vector4(r, g, b, 1.0f);
			}
		}
	}

	Texture3dConfigDX11 tex3dConfig;
	tex3dConfig.SetWidth(SCATTERING_TEXTURE_WIDTH);
	tex3dConfig.SetHeight(SCATTERING_TEXTURE_HEIGHT);
	tex3dConfig.SetDepth(SCATTERING_TEXTURE_DEPTH);
	tex3dConfig.SetFormat(DXGI_FORMAT_R32G32B32A32_FLOAT);
	tex3dConfig.SetUsage(D3D11_USAGE_DYNAMIC);
	tex3dConfig.SetCPUAccessFlags(D3D11_CPU_ACCESS_WRITE);

	D3D11_SUBRESOURCE_DATA subResource;
	subResource.pSysMem = &rgbScatteringFloatBuffer[0];
	subResource.SysMemPitch = SCATTERING_TEXTURE_WIDTH * 4 * sizeof(float);
	subResource.SysMemSlicePitch = SCATTERING_TEXTURE_HEIGHT * SCATTERING_TEXTURE_WIDTH * 4 * sizeof(float);
	uint32_t textureHandle = TextureDX11ResourceFactory::getInstance().createTexture3D("Scattering_Texture", tex3dConfig, &subResource);

	m_scattering3DTexture = (Texture3dDX11*)TextureDX11ResourceFactory::getInstance().getResource(textureHandle);

	//single mie scattering texture	
	for (int k = 0; k < SCATTERING_TEXTURE_DEPTH; ++k)
	{
		float fk = (float)(k + 0.5) / SCATTERING_TEXTURE_DEPTH;

		for (int j = 0; j < SCATTERING_TEXTURE_HEIGHT; ++j)
		{
			float fj = (float)(j + 0.5) / SCATTERING_TEXTURE_HEIGHT;

			for (int i = 0; i < SCATTERING_TEXTURE_WIDTH; ++i)
			{
				float fi = (float)(i + 0.5) / SCATTERING_TEXTURE_WIDTH;

				float3 uvw(fi, fj, fk);
				IrradianceSpectrum irradiacne = lookupScatteringTexture(*m_single_mie_scattering_texture.get(), uvw);
				float r = irradiacne(kLambdaR).to(watt_per_square_meter_per_nm);
				float g = irradiacne(kLambdaG).to(watt_per_square_meter_per_nm);
				float b = irradiacne(kLambdaB).to(watt_per_square_meter_per_nm);

				size_t index = k * (SCATTERING_TEXTURE_HEIGHT * SCATTERING_TEXTURE_WIDTH) +
					j * SCATTERING_TEXTURE_WIDTH + i;
				rgbScatteringFloatBuffer[index] = Vector4(r, g, b, 1.0f);
			}
		}
	}

	tex3dConfig.SetWidth(SCATTERING_TEXTURE_WIDTH);
	tex3dConfig.SetHeight(SCATTERING_TEXTURE_HEIGHT);
	tex3dConfig.SetDepth(SCATTERING_TEXTURE_DEPTH);
	tex3dConfig.SetFormat(DXGI_FORMAT_R32G32B32A32_FLOAT);
	tex3dConfig.SetUsage(D3D11_USAGE_DYNAMIC);
	tex3dConfig.SetCPUAccessFlags(D3D11_CPU_ACCESS_WRITE);

	subResource.pSysMem = &rgbScatteringFloatBuffer[0];
	subResource.SysMemPitch = SCATTERING_TEXTURE_WIDTH * 4 * sizeof(float);
	subResource.SysMemSlicePitch = SCATTERING_TEXTURE_HEIGHT * SCATTERING_TEXTURE_WIDTH * 4 * sizeof(float);
	textureHandle = TextureDX11ResourceFactory::getInstance().createTexture3D("SingleMieScattering_Texture", tex3dConfig, &subResource);

	m_singleMieScattering3DTexture = (Texture3dDX11*)TextureDX11ResourceFactory::getInstance().getResource(textureHandle);
}


//float g_progress = 0.0f;
//std::atomic_int g_atomicProgress = 0;
//std::atomic_int g_freeThread = 0;

void SkyRenderTest::updateSunDirection()
{
	//¶Èµ½»¡¶È
	float sunTheta_radian = m_sunTheta * PI / 180.0f;
	float sunphi = m_sunPhi * PI / 180.0f;;

	m_sunDirection.m_y = cos(sunTheta_radian);
	m_sunDirection.m_x = sin(sunTheta_radian) * cos(sunphi);
	m_sunDirection.m_z = sin(sunTheta_radian) * sin(sunphi);
	m_sunDirection.normalize();
}


void SkyRenderTest::renderSky()
{
	updateSunDirection();
	
}

void SkyRenderTest::onGUI()
{
	updateSunDirection();

	static bool g_overLayShow = true;
	ImGui::SetNextWindowPos(ImVec2(100, 300));
	if (!ImGui::Begin("SkyRender", &g_overLayShow, ImVec2(500, 800), 0.7f, ImGuiWindowFlags_NoTitleBar))
	{
		ImGui::End();
		return;
	}

	if (ImGui::Button("CPU Render", ImVec2(200, 50)))
	{
		m_isProcessing = true;
		m_skyRenderCPUTest->renderSky(m_camera, m_sunDirection);
	}

	ImGui::SliderFloat("sun theta", &m_sunTheta, 0.0f, 180);

	ImGui::SliderFloat("sun phi", &m_sunPhi, 0.0f, 360.0f);

	/*if (m_isProcessing)
	{
		float fProgress = (float)g_atomicProgress / (m_windowHeight * m_windowWidth);
		ImGui::ProgressBar(fProgress, ImVec2(150, 20));
		if (fProgress >= 1.0f)
		{
			m_isProcessing = false;

			updateCpuSkyTexture();
		}
	}*/

	m_cpuSkyTextureHandle = m_skyRenderCPUTest->getSkyRenderTextureHandle();
	if (m_cpuSkyTextureHandle != 0)
	{
		Texture2dDX11* srcTex2d = (Texture2dDX11*)TextureDX11ResourceFactory::getInstance().getResource(m_cpuSkyTextureHandle);

		ImGui::Image(srcTex2d->getShaderResourceView(), ImVec2(200, 200));
	}


	ImGui::End();
}

NAME_SPACE_END
NAME_SPACE_END
NAME_SPACE_END