#include "stdafx.h"
#include "SkyRenderCPUTest.h"
#include "Camera.h"
#include "Environment/Atmosphere/Reference/Functions.h"
//#include "Environment/Atmosphere/Reference/Definitions.h"
#include "Environment/Atmosphere/Constants.h"
#include "Texture2dConfigDX11.h"
#include "TextureDX11ResourceFactory.h"
#include "RendererDX11.h"
#include "Texture2dDX11.h"

#define _IN(x) const x&
#define _OUT(x) x&
#define TEMPLATE(x) template<class x>
#define TEMPLATE_ARGUMENT(x) <x>



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

SkyRenderCPUTest::SkyRenderCPUTest(int w, int h, Camera* camera)
{
	m_camera = camera;
	m_windowWidth = w;
	m_windowHeight = h;
}

void SkyRenderCPUTest::init()
{
	// Values from "Reference Solar Spectral Irradiance: ASTM G-173", ETR column
	// (see http://rredc.nrel.gov/solar/spectra/am1.5/ASTMG173/ASTMG173.html),
	// summed and averaged in each bin (e.g. the value for 360nm is the average
	// of the ASTM G-173 values for all wavelengths between 360 and 370nm).
	// Values in W.m^-2.
	constexpr int kLambdaMin = 360;
	constexpr int kLambdaMax = 830;
	constexpr double kSolarIrradiance[48] = 
	{
		1.11776, 1.14259, 1.01249, 1.14716, 1.72765, 1.73054, 1.6887, 1.61253,
		1.91198, 2.03474, 2.02042, 2.02212, 1.93377, 1.95809, 1.91686, 1.8298,
		1.8685, 1.8931, 1.85149, 1.8504, 1.8341, 1.8345, 1.8147, 1.78158, 1.7533,
		1.6965, 1.68194, 1.64654, 1.6048, 1.52143, 1.55622, 1.5113, 1.474, 1.4482,
		1.41018, 1.36775, 1.34188, 1.31429, 1.28303, 1.26758, 1.2367, 1.2082,
		1.18737, 1.14683, 1.12362, 1.1058, 1.07124, 1.04992
	};
	constexpr ScatteringCoefficient kRayleigh = 1.24062e-6 / m;
	constexpr Length kRayleighScaleHeight = 8000.0 * m;
	constexpr Length kMieScaleHeight = 1200.0 * m;
	constexpr double kMieAngstromAlpha = 0.0;
	constexpr double kMieAngstromBeta = 5.328e-3;
	constexpr double kMieSingleScatteringAlbedo = 0.9;
	constexpr double kMiePhaseFunctionG = 0.8;
	// Values from http://www.iup.uni-bremen.de/gruppen/molspec/databases/
	// referencespectra/o3spectra2011/index.html for 233K, summed and averaged
	// in each bin (e.g. the value for 360nm is the average of the original
	// values for all wavelengths between 360 and 370nm). Values in m^2.
	constexpr double kOzoneCrossSection[48] = 
	{
		1.18e-27, 2.182e-28, 2.818e-28, 6.636e-28, 1.527e-27, 2.763e-27, 5.52e-27,
		8.451e-27, 1.582e-26, 2.316e-26, 3.669e-26, 4.924e-26, 7.752e-26,
		9.016e-26, 1.48e-25, 1.602e-25, 2.139e-25, 2.755e-25, 3.091e-25, 3.5e-25,
		4.266e-25, 4.672e-25, 4.398e-25, 4.701e-25, 5.019e-25, 4.305e-25,
		3.74e-25, 3.215e-25, 2.662e-25, 2.238e-25, 1.852e-25, 1.473e-25,
		1.209e-25, 9.423e-26, 7.455e-26, 6.566e-26, 5.105e-26, 4.15e-26,
		4.228e-26, 3.237e-26, 2.451e-26, 2.801e-26, 2.534e-26, 1.624e-26,
		1.465e-26, 2.078e-26, 1.383e-26, 7.105e-27
	};
	// From https://en.wikipedia.org/wiki/Dobson_unit, in molecules.m^-2.
	constexpr dimensional::Scalar<-2, 0, 0, 0, 0> kDobsonUnit = 2.687e20 / m2;
	// Maximum number density of ozone molecules, in m^-3 (computed so at to get
	// 300 Dobson units of ozone - for this we divide 300 DU by the integral of
	// the ozone density profile defined below, which is equal to 15km).
	/*
	臭氧分子的最大数密度，单位为m ^ -3（臭氧的300多布森单位 - 为此，我们将300 DU除以下面定义的臭氧密度分布的积分，其等于15km）
	*/
	constexpr NumberDensity kMaxOzoneNumberDensity = 300.0 * kDobsonUnit / (15.0 * km);

	std::vector<SpectralIrradiance> solar_irradiance;
	std::vector<ScatteringCoefficient> rayleigh_scattering;
	std::vector<ScatteringCoefficient> mie_scattering;
	std::vector<ScatteringCoefficient> mie_extinction;
	std::vector<ScatteringCoefficient> absorption_extinction;
	
	for (int l = kLambdaMin; l <= kLambdaMax; l += 10) 
	{
		double lambda = static_cast<double>(l) * 1e-3;  // micro-meters
		double fff = pow(lambda, -4);
		SpectralIrradiance solar = kSolarIrradiance[(l - kLambdaMin) / 10] * watt_per_square_meter_per_nm;
		ScatteringCoefficient rayleigh = kRayleigh * pow(lambda, -4);
		ScatteringCoefficient mie = kMieAngstromBeta / kMieScaleHeight * pow(lambda, -kMieAngstromAlpha);
		solar_irradiance.push_back(solar);
		rayleigh_scattering.push_back(rayleigh);
		mie_scattering.push_back(mie * kMieSingleScatteringAlbedo);
		mie_extinction.push_back(mie);
		absorption_extinction.push_back(kMaxOzoneNumberDensity * kOzoneCrossSection[(l - kLambdaMin) / 10] * m2);
	}

	m_atmosphereParameters.solar_irradiance = IrradianceSpectrum(kLambdaMin * nm, kLambdaMax * nm, solar_irradiance);
	m_atmosphereParameters.sun_angular_radius = 0.2678 * deg;
	m_atmosphereParameters.bottom_radius = 6360.0 * km;
	m_atmosphereParameters.top_radius = 6420.0 * km;
	m_atmosphereParameters.rayleigh_density.layers[1] = DensityProfileLayer(0.0 * m, 1.0, -1.0 / kRayleighScaleHeight, 0.0 / m, 0.0);
	m_atmosphereParameters.rayleigh_scattering = ScatteringSpectrum(kLambdaMin * nm, kLambdaMax * nm, rayleigh_scattering);
	m_atmosphereParameters.mie_density.layers[1] = DensityProfileLayer(0.0 * m, 1.0, -1.0 / kMieScaleHeight, 0.0 / m, 0.0);
	m_atmosphereParameters.mie_scattering = ScatteringSpectrum(kLambdaMin * nm, kLambdaMax * nm, mie_scattering);
	m_atmosphereParameters.mie_extinction = ScatteringSpectrum(kLambdaMin * nm, kLambdaMax * nm, mie_extinction);
	m_atmosphereParameters.mie_phase_function_g = kMiePhaseFunctionG;
	// Density profile increasing linearly from 0 to 1 between 10 and 25km, and
	// decreasing linearly from 1 to 0 between 25 and 40km. Approximate profile
	// from http://www.kln.ac.lk/science/Chemistry/Teaching_Resources/Documents/
	// Introduction%20to%20atmospheric%20chemistry.pdf (page 10).
	m_atmosphereParameters.absorption_density.layers[0] = DensityProfileLayer(
		25.0 * km, 0.0, 0.0 / km, 1.0 / (15.0 * km), -2.0 / 3.0);
	m_atmosphereParameters.absorption_density.layers[1] = DensityProfileLayer(
		0.0 * km, 0.0, 0.0 / km, -1.0 / (15.0 * km), 8.0 / 3.0);
	m_atmosphereParameters.absorption_extinction = ScatteringSpectrum(
		kLambdaMin * nm, kLambdaMax * nm, absorption_extinction);
	m_atmosphereParameters.ground_albedo = DimensionlessSpectrum(0.1);
	m_atmosphereParameters.mu_s_min = cos(102.0 * deg);
}

Vector3 SkyRenderCPUTest::uvToCameraRay(Vector2 inUV, const Matrix4x4& projMat, const Matrix4x4& inverseViewMat)
{
	Vector2 uv = inUV;
	uv.m_y = 1.0f - uv.m_y;
	uv = uv * 2.0f - Vector2(1.0, 1.0);
	Vector4 clipPos = Vector4(uv.m_x, uv.m_y, 0.5, 1.0);

	Vector3 ray;
	ray.m_z = (clipPos.m_z - projMat.m_matrix[3][2]) / projMat.m_matrix[2][2];
	ray.m_x = clipPos.m_x * ray.m_z * projMat.m_matrix[2][3];
	ray.m_y = clipPos.m_y * ray.m_z * projMat.m_matrix[2][3] / projMat.m_matrix[1][1];
	ray.normalize();

	Vector4 temp(ray.m_x, ray.m_y, ray.m_z, 0.0f);
	temp = temp * inverseViewMat;
	return Vector3(temp.m_x, temp.m_y, temp.m_z);
}


void SkyRenderCPUTest::renderSingleScatting()
{
	Vector3	sunDirection(1, 1, 1);
	sunDirection.normalize();
	Matrix4x4 projMat = m_camera->getProjMat();
	Matrix4x4 inverseViewMat = m_camera->getViewMat().inverse();

	double bottom_radius = m_atmosphereParameters.bottom_radius.to(m);
	float top_radius = m_atmosphereParameters.top_radius.to(m);
	Vector3 worldPosVec3 = m_camera->getPosition();
	Vector3 earthSpacePosVec3 = (m_camera->getPosition() + Vector3(0.0, bottom_radius, 0.0f));

	if (m_radianceRGBBuffer.size() != m_windowHeight * m_windowHeight)
		m_radianceRGBBuffer.resize(m_windowHeight * m_windowHeight);

	for (int y = 0; y < m_windowHeight; ++y)
	{
		for (int x = 0; x < m_windowWidth; ++x)
		{
			float u = (float)x / m_windowWidth;
			float v = (float)y / m_windowHeight;

			Vector3 ray = uvToCameraRay(Vector2(u, v), projMat, inverseViewMat);
			ray.normalize();
			double r = earthSpacePosVec3.length();
			Vector3 earthCenterToEyeDirection = earthSpacePosVec3 / r;
			double mu = ray.dot(earthCenterToEyeDirection);
			double mu_s = sunDirection.dot(earthCenterToEyeDirection);
			double nu = ray.dot(sunDirection);
			bool rayIsIntersectsGround = RayIntersectsGround(m_atmosphereParameters, r * m,mu);

			RadianceSpectrum rgbSpectrum = computeSingleScatting(m_atmosphereParameters,r*m, mu, mu_s, nu, rayIsIntersectsGround);
			
			double color_r = rgbSpectrum(kLambdaR).to(watt_per_square_meter_per_sr_per_nm);
			double color_g = rgbSpectrum(kLambdaG).to(watt_per_square_meter_per_sr_per_nm);
			double color_b = rgbSpectrum(kLambdaB).to(watt_per_square_meter_per_sr_per_nm);

			Vector3 color(color_r, color_g, color_b);

			m_radianceRGBBuffer[y * m_windowWidth + x] = color;
		}
	}

	
	saveRadianceRGBBufferToFile();
}

void SkyRenderCPUTest::saveRadianceRGBBufferToFile()
{
	//创建纹理
	Texture2dConfigDX11 tex2dConfig;
	tex2dConfig.SetWidth(m_windowWidth);
	tex2dConfig.SetHeight(m_windowHeight);
	tex2dConfig.SetBindFlags(D3D11_BIND_SHADER_RESOURCE);
	tex2dConfig.SetFormat(DXGI_FORMAT_R8G8B8A8_UNORM);
	tex2dConfig.SetUsage(D3D11_USAGE_DYNAMIC);
	uint32_t handle = TextureDX11ResourceFactory::getInstance().createTexture2D("RadianceRGBTexture", tex2dConfig);

	Texture2dDX11* rgbTex = (Texture2dDX11*)TextureDX11ResourceFactory::getInstance().getResource(handle);

	D3D11_MAPPED_SUBRESOURCE mapSubResource;
	mapSubResource.pData = &m_radianceRGBBuffer[0];
	mapSubResource.RowPitch = m_windowWidth * 4;
	mapSubResource.DepthPitch = 0;
	HRESULT hr = RendererDX11::getInstance().getDeviceContex()->Map(rgbTex->getTexture2D(), 0, D3D11_MAP_WRITE, 0, &mapSubResource);

	RendererDX11::getInstance().getDeviceContex()->Unmap(rgbTex->getTexture2D(), 0);

	D3DX11SaveTextureToFile()
}

NAME_SPACE_END
NAME_SPACE_END
NAME_SPACE_END