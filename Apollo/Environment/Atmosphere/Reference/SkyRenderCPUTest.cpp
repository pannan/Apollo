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
//#include "SDK/minpng.h"
#include <thread>
#include "imgui.h"
#include<atomic>  
#include "TextureDX11ResourceFactory.h"
#include "Texture3dConfigDX11.h"
#include "Environment/Atmosphere/Reference/Definitions.h"

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

SkyRenderCPUTest::SkyRenderCPUTest(int w, int h)
{
	m_isProcessing = false;
	m_windowWidth = w;
	m_windowHeight = h;

//	m_sunTheta = 85.0f;
	//m_sunPhi = 0.0f;

	m_cpuSkyTextureHandle = 0;

	Vector3 camPos(0, 1000, 0);
	Vector3 lookAt = camPos + Vector3(3, 8, 1) * 10;
	//m_camera = camera;
}

void SkyRenderCPUTest::init(std::shared_ptr<ReducedScatteringTexture>& scatteringTexture,
	std::shared_ptr<ReducedScatteringTexture>& singleMieScatteringTexture)
{
	m_scattering_texture = scatteringTexture;
	m_single_mie_scattering_texture = singleMieScatteringTexture; m_scattering_texture;

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

	//initLookupTexture();
}

void SkyRenderCPUTest::initLookupTexture()
{
	//transmittance_texture_.reset(new TransmittanceTexture());
	//m_scattering_texture.reset(new ReducedScatteringTexture());
	//m_single_mie_scattering_texture.reset(new ReducedScatteringTexture());

	//const size_t testSize = sizeof(IrradianceSpectrum);
	//std::ifstream file;
	//file.open("h:\\scattering.dat");
	//if (!file.good())
	//	return;


	//file.close();
	////transmittance_texture_->Load(cache_directory_ + "transmittance.dat");
	//m_scattering_texture->Load("h:\\scattering.dat");
	//m_single_mie_scattering_texture->Load("h:\\single_mie_scattering.dat");
	////irradiance_texture_->Load(cache_directory_ + "irradiance.dat");


	//createScatteringTextureFromMemoryBuffer();
}

//void SkyRenderCPUTest::createScatteringTextureFromMemoryBuffer()
//{
//	//scattering texture
//	std::vector<Vector4> rgbScatteringFloatBuffer;
//	rgbScatteringFloatBuffer.resize(SCATTERING_TEXTURE_WIDTH * SCATTERING_TEXTURE_HEIGHT * SCATTERING_TEXTURE_DEPTH);
//
//	for (int k = 0; k < SCATTERING_TEXTURE_DEPTH; ++k)
//	{
//		float fk = (float)(k + 0.5) / SCATTERING_TEXTURE_DEPTH;
//
//		for (int j = 0; j < SCATTERING_TEXTURE_HEIGHT; ++j)
//		{
//			float fj = (float)(j + 0.5) / SCATTERING_TEXTURE_HEIGHT;
//
//			for (int i = 0; i < SCATTERING_TEXTURE_WIDTH; ++i)
//			{
//				float fi = (float)(i + 0.5) / SCATTERING_TEXTURE_WIDTH;
//
//				float3 uvw(fi, fj, fk);
//				IrradianceSpectrum irradiacne = lookupScatteringTexture(*m_scattering_texture.get(), uvw);
//				float r = irradiacne(kLambdaR).to(watt_per_square_meter_per_nm);
//				float g = irradiacne(kLambdaG).to(watt_per_square_meter_per_nm);
//				float b = irradiacne(kLambdaB).to(watt_per_square_meter_per_nm);
//
//				size_t index = k * (SCATTERING_TEXTURE_HEIGHT * SCATTERING_TEXTURE_WIDTH) +
//					j * SCATTERING_TEXTURE_WIDTH + i;
//				rgbScatteringFloatBuffer[index] = Vector4(r, g, b, 1.0f);
//			}
//		}
//	}
//
//	Texture3dConfigDX11 tex3dConfig;
//	tex3dConfig.SetWidth(SCATTERING_TEXTURE_WIDTH);
//	tex3dConfig.SetHeight(SCATTERING_TEXTURE_HEIGHT);
//	tex3dConfig.SetDepth(SCATTERING_TEXTURE_DEPTH);
//	tex3dConfig.SetFormat(DXGI_FORMAT_R32G32B32A32_FLOAT);
//	tex3dConfig.SetUsage(D3D11_USAGE_DYNAMIC);
//	tex3dConfig.SetCPUAccessFlags(D3D11_CPU_ACCESS_WRITE);
//
//	D3D11_SUBRESOURCE_DATA subResource;
//	subResource.pSysMem = &rgbScatteringFloatBuffer[0];
//	subResource.SysMemPitch = SCATTERING_TEXTURE_WIDTH * 4 * sizeof(float);
//	subResource.SysMemSlicePitch = SCATTERING_TEXTURE_HEIGHT * SCATTERING_TEXTURE_WIDTH * 4 * sizeof(float);
//	uint32_t textureHandle = TextureDX11ResourceFactory::getInstance().createTexture3D("Scattering_Texture", tex3dConfig, &subResource);
//
//	m_scattering3DTexture = (Texture3dDX11*)TextureDX11ResourceFactory::getInstance().getResource(textureHandle);
//
//	//single mie scattering texture	
//	for (int k = 0; k < SCATTERING_TEXTURE_DEPTH; ++k)
//	{
//		float fk = (float)(k + 0.5) / SCATTERING_TEXTURE_DEPTH;
//
//		for (int j = 0; j < SCATTERING_TEXTURE_HEIGHT; ++j)
//		{
//			float fj = (float)(j + 0.5) / SCATTERING_TEXTURE_HEIGHT;
//
//			for (int i = 0; i < SCATTERING_TEXTURE_WIDTH; ++i)
//			{
//				float fi = (float)(i + 0.5) / SCATTERING_TEXTURE_WIDTH;
//
//				float3 uvw(fi, fj, fk);
//				IrradianceSpectrum irradiacne = lookupScatteringTexture(*m_single_mie_scattering_texture.get(), uvw);
//				float r = irradiacne(kLambdaR).to(watt_per_square_meter_per_nm);
//				float g = irradiacne(kLambdaG).to(watt_per_square_meter_per_nm);
//				float b = irradiacne(kLambdaB).to(watt_per_square_meter_per_nm);
//
//				size_t index = k * (SCATTERING_TEXTURE_HEIGHT * SCATTERING_TEXTURE_WIDTH) +
//					j * SCATTERING_TEXTURE_WIDTH + i;
//				rgbScatteringFloatBuffer[index] = Vector4(r, g, b, 1.0f);
//			}
//		}
//	}
//
//	tex3dConfig.SetWidth(SCATTERING_TEXTURE_WIDTH);
//	tex3dConfig.SetHeight(SCATTERING_TEXTURE_HEIGHT);
//	tex3dConfig.SetDepth(SCATTERING_TEXTURE_DEPTH);
//	tex3dConfig.SetFormat(DXGI_FORMAT_R32G32B32A32_FLOAT);
//	tex3dConfig.SetUsage(D3D11_USAGE_DYNAMIC);
//	tex3dConfig.SetCPUAccessFlags(D3D11_CPU_ACCESS_WRITE);
//
//	subResource.pSysMem = &rgbScatteringFloatBuffer[0];
//	subResource.SysMemPitch = SCATTERING_TEXTURE_WIDTH * 4 * sizeof(float);
//	subResource.SysMemSlicePitch = SCATTERING_TEXTURE_HEIGHT * SCATTERING_TEXTURE_WIDTH * 4 * sizeof(float);
//	textureHandle = TextureDX11ResourceFactory::getInstance().createTexture3D("SingleMieScattering_Texture", tex3dConfig, &subResource);
//
//	m_singleMieScattering3DTexture = (Texture3dDX11*)TextureDX11ResourceFactory::getInstance().getResource(textureHandle);
//}

Vector3 uvToCameraRay(Vector2 inUV, const Matrix4x4& projMat, const Matrix4x4& inverseViewMat)
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

struct RayRadianceThreadChunk 
{
	Vector3 earthSpacePosVec3;
	Vector3 sunDirection;
	AtmosphereParameters atmosphereParameters;
	Matrix4x4 projMat;
	Matrix4x4 inverseViewMat;
};

//outRadiance不能为Vector3&这样的引用 ，不然不发传递值，要为指针
void computeRayRadianceThread(const Vector2& uv, RayRadianceThreadChunk& threadChunk,Vector3* outRadiance)
{
	Vector3 ray = uvToCameraRay(uv, threadChunk.projMat, threadChunk.inverseViewMat);
	ray.normalize();
	double r = threadChunk.earthSpacePosVec3.length();
	Vector3 earthCenterToEyeDirection = threadChunk.earthSpacePosVec3 / r;
	double mu = ray.dot(earthCenterToEyeDirection);
	double mu_s = threadChunk.sunDirection.dot(earthCenterToEyeDirection);
	double nu = ray.dot(threadChunk.sunDirection);
	bool rayIsIntersectsGround = RayIntersectsGround(threadChunk.atmosphereParameters, r * m, mu);
	if (rayIsIntersectsGround)
		int ii = 0;
	RadianceSpectrum rgbSpectrum = computeSingleScatting(threadChunk.atmosphereParameters, r*m, mu, mu_s, nu, rayIsIntersectsGround);

	double color_r = rgbSpectrum(kLambdaR).to(watt_per_square_meter_per_sr_per_nm);
	double color_g = rgbSpectrum(kLambdaG).to(watt_per_square_meter_per_sr_per_nm);
	double color_b = rgbSpectrum(kLambdaB).to(watt_per_square_meter_per_sr_per_nm);

	Number exposure_ = 10;
	double testg = 1.0 - std::exp(-color_g * exposure_());
	color_r = std::pow(1.0 - std::exp(-color_r * exposure_()), 1.0 / 2.2);
	color_g = std::pow(1.0 - std::exp(-color_g * exposure_()), 1.0 / 2.2);
	color_b = std::pow(1.0 - std::exp(-color_b * exposure_()), 1.0 / 2.2);

	*outRadiance = Vector3(color_r, color_g, color_b);
}

Vector2	normalizeUV(float x, float y, int w, int h)
{
	float u = (float)x / w;
	float v = (float)y / h;
	return Vector2(u, v);
}

float g_progress = 0.0f;
std::atomic_int g_atomicProgress = 0;
std::atomic_int g_freeThread = 0;


void computeSkyRadianceThread(int w,int h, RayRadianceThreadChunk& threadChunk, Vector3* outRadiance)
{
	for (int y = 0; y < h; ++y)
	{
		for (int x = 0; x < w;)
		{
			int pixelXIndex[4];
			Vector2 uv[4];

			pixelXIndex[0] = x;
			uv[0] = normalizeUV(x, y,w,h);

			++x;
			pixelXIndex[1] = x % w;
			uv[1] = normalizeUV(pixelXIndex[1], y,w,h);

			++x;
			pixelXIndex[2] = x % w;
			uv[2] = normalizeUV(pixelXIndex[2], y, w, h);

			++x;
			pixelXIndex[3] = x % w;
			uv[3] = normalizeUV(pixelXIndex[3], y, w, h);

			++x;

			Vector3 tempRadiance[4];

			std::thread t0(computeRayRadianceThread, uv[0], threadChunk, &tempRadiance[0]);
			std::thread t1(computeRayRadianceThread, uv[1], threadChunk, &tempRadiance[1]);
			std::thread t2(computeRayRadianceThread, uv[2], threadChunk, &tempRadiance[2]);
			std::thread t3(computeRayRadianceThread, uv[3], threadChunk, &tempRadiance[3]);

			t0.join();
			t1.join();
			t2.join();
			t3.join();

			outRadiance[y * w + pixelXIndex[0]] = tempRadiance[0];
			outRadiance[y * w + pixelXIndex[1]] = tempRadiance[1];
			outRadiance[y * w + pixelXIndex[2]] = tempRadiance[2];
			outRadiance[y * w + pixelXIndex[3]] = tempRadiance[3];

			g_atomicProgress += 4;
		}
	}

	int ii = 0;
}

//这里的outRadiance是个数组
void computeRayRadianceThread2(int x, int y, int w, int h, RayRadianceThreadChunk& threadChunk, Vector3* outRadiance)
{
	Vector2 uv = normalizeUV(x, y, w, h);
	Vector3 ray = uvToCameraRay(uv, threadChunk.projMat, threadChunk.inverseViewMat);
	ray.normalize();
	double r = threadChunk.earthSpacePosVec3.length();
	Vector3 earthCenterToEyeDirection = threadChunk.earthSpacePosVec3 / r;
	double mu = ray.dot(earthCenterToEyeDirection);
	double mu_s = threadChunk.sunDirection.dot(earthCenterToEyeDirection);
	double nu = ray.dot(threadChunk.sunDirection);
	bool rayIsIntersectsGround = RayIntersectsGround(threadChunk.atmosphereParameters, r * m, mu);
	if (rayIsIntersectsGround)
		int ii = 0;
	RadianceSpectrum rgbSpectrum = computeSingleScatting(threadChunk.atmosphereParameters, r*m, mu, mu_s, nu, rayIsIntersectsGround);

	double color_r = rgbSpectrum(kLambdaR).to(watt_per_square_meter_per_sr_per_nm);
	double color_g = rgbSpectrum(kLambdaG).to(watt_per_square_meter_per_sr_per_nm);
	double color_b = rgbSpectrum(kLambdaB).to(watt_per_square_meter_per_sr_per_nm);

	Number exposure_ = 10;
	double testg = 1.0 - std::exp(-color_g * exposure_());
	color_r = std::pow(1.0 - std::exp(-color_r * exposure_()), 1.0 / 2.2);
	color_g = std::pow(1.0 - std::exp(-color_g * exposure_()), 1.0 / 2.2);
	color_b = std::pow(1.0 - std::exp(-color_b * exposure_()), 1.0 / 2.2);

	outRadiance[y * w + x] = Vector3(color_r, color_g, color_b);
	++g_atomicProgress;
	++g_freeThread;
}

void computeSkyRadianceThread2(int w, int h, RayRadianceThreadChunk& threadChunk, Vector3* outRadiance)
{
	const size_t totalPixel = w * h;
	const int cpuCount = std::thread::hardware_concurrency();
	g_freeThread = cpuCount;
	g_atomicProgress = 0;

	while (g_atomicProgress < totalPixel)
	{
		if (g_freeThread > 0)
		{
			--g_freeThread;
			//分配一个线程计算
			int indexX = g_atomicProgress % w;
			int indexY = g_atomicProgress / w;

			std::thread t(computeRayRadianceThread2,indexX,indexY,w,h, threadChunk, outRadiance);

			t.detach();
		}
	}
}

//void SkyRenderCPUTest::updateSunDirection()
//{
//	//度到弧度
//	float sunTheta_radian = m_sunTheta * PI / 180.0f;
//	float sunphi = m_sunPhi * PI / 180.0f;;
//
//	m_sunDirection.m_y = cos(sunTheta_radian);
//	m_sunDirection.m_x = sin(sunTheta_radian) * cos(sunphi);
//	m_sunDirection.m_z = sin(sunTheta_radian) * sin(sunphi);
//	m_sunDirection.normalize();
//}

//void SkyRenderCPUTest::checkRMuMusNuConversion()
//{
//	updateSunDirection();
//
//	Matrix4x4 projMat = m_camera->getProjMat();
//	Matrix4x4 inverseViewMat = m_camera->getViewMat().inverse();
//
//
//	for (int y = 0; y < m_windowHeight; ++y)
//	{
//		for (int x = 0; x < m_windowWidth; ++x)
//		{
//			float u = (float)x / m_windowWidth;
//			float v = (float)y / m_windowHeight;
//
//			Vector3 ray = uvToCameraRay(Vector2(u, v), projMat, inverseViewMat);
//			ray.normalize();
//			double r = m_earthSpacePosVec3.length();
//			Vector3 earthCenterToEyeDirection = m_earthSpacePosVec3 / r;
//			double mu = ray.dot(earthCenterToEyeDirection);
//			double mu_s = m_sunDirection.dot(earthCenterToEyeDirection);
//			double nu = ray.dot(m_sunDirection);
//			bool rayIsIntersectsGround = RayIntersectsGround(m_atmosphereParameters, r * m, mu);
//
//			testRMuMusNuConversion(m_atmosphereParameters, r*m, mu, mu_s, nu, rayIsIntersectsGround);
//		}
//	}
//}

//#define  ENABLE_THREAD

void SkyRenderCPUTest::renderSky(Camera* camera,Vector3 sunDirection)
{
	g_atomicProgress = 0;

	//updateSunDirection();

	Matrix4x4 projMat = camera->getProjMat();
	Matrix4x4 inverseViewMat = camera->getViewMat().inverse();

	double bottom_radius = m_atmosphereParameters.bottom_radius.to(m);
	float top_radius = m_atmosphereParameters.top_radius.to(m);
	Vector3 worldPosVec3 = camera->getPosition();
	m_earthSpacePosVec3 = (camera->getPosition() + Vector3(0.0, bottom_radius, 0.0f));

	if (m_radianceRGBBuffer.size() != m_windowWidth * m_windowHeight)
		m_radianceRGBBuffer.resize(m_windowWidth * m_windowHeight);

	RayRadianceThreadChunk threadChunk;
	threadChunk.atmosphereParameters = m_atmosphereParameters;
	threadChunk.earthSpacePosVec3 = m_earthSpacePosVec3;
	threadChunk.inverseViewMat = inverseViewMat;
	threadChunk.projMat = projMat;
	threadChunk.sunDirection = sunDirection;

	//checkRMuMusNuConversion();
	//return;

#ifdef ENABLE_THREAD

	std::thread t(computeSkyRadianceThread,m_windowWidth,m_windowHeight,threadChunk, &m_radianceRGBBuffer[0]);
	t.detach();
	

#else

	//m_isProcessing = true;
	LazyTransmittanceTexture transmittance_texture(m_atmosphereParameters);
	LazySingleScatteringTexture single_rayleigh_scattering_texture(m_atmosphereParameters, transmittance_texture, true);
	LazySingleScatteringTexture single_mie_scattering_texture(m_atmosphereParameters, transmittance_texture, false);

	for (int y = 0; y < m_windowHeight; ++y)
	{
		for (int x = 0; x < m_windowWidth; ++x)
		{
			float u = (float)x / m_windowWidth;
			float v = (float)y / m_windowHeight;			

			Vector3 ray = uvToCameraRay(Vector2(u, v), projMat, inverseViewMat);
			ray.normalize();
			double r = m_earthSpacePosVec3.length();
			Vector3 earthCenterToEyeDirection = m_earthSpacePosVec3 / r;
			double mu = ray.dot(earthCenterToEyeDirection);
			double mu_s = sunDirection.dot(earthCenterToEyeDirection);
			double nu = ray.dot(sunDirection);
			bool rayIsIntersectsGround = RayIntersectsGround(m_atmosphereParameters, r * m,mu);
			if (rayIsIntersectsGround)
				int ii = 0;
			//RadianceSpectrum rgbSpectrum = recomputeSingleScatting(m_atmosphereParameters,r*m, mu, mu_s, nu, rayIsIntersectsGround);
			//RadianceSpectrum rgbSpectrum = recomputeSingleScatting(m_atmosphereParameters, r*m, mu, mu_s, nu, rayIsIntersectsGround,
			//	transmittance_texture, single_rayleigh_scattering_texture, single_mie_scattering_texture);
			RadianceSpectrum rgbSpectrum = getSkyScatting(m_atmosphereParameters, r*m, mu, mu_s, nu, rayIsIntersectsGround,
				*m_scattering_texture.get(), *m_single_mie_scattering_texture.get());
			//RadianceSpectrum rgbSpectrum = computeSingleScatting(m_atmosphereParameters, r*m, mu, mu_s, nu, rayIsIntersectsGround);
			
			double color_r = rgbSpectrum(kLambdaR).to(watt_per_square_meter_per_sr_per_nm);
			double color_g = rgbSpectrum(kLambdaG).to(watt_per_square_meter_per_sr_per_nm);
			double color_b = rgbSpectrum(kLambdaB).to(watt_per_square_meter_per_sr_per_nm);

			Number exposure_ = 10;
			double testg = 1.0 - std::exp(-color_g * exposure_());
			color_r = std::pow(1.0 - std::exp(-color_r * exposure_()), 1.0 / 2.2);
			color_g = std::pow(1.0 - std::exp(-color_g * exposure_()), 1.0 / 2.2);
			color_b = std::pow(1.0 - std::exp(-color_b * exposure_()), 1.0 / 2.2);

			Vector3 color(color_r, color_g, color_b);

			m_radianceRGBBuffer[y * m_windowWidth + x] = color;

		//	++g_atomicProgress;
		}
	}

	updateCpuSkyTexture();

#endif
}

void SkyRenderCPUTest::updateCpuSkyTexture()
{
	rgbaFloatBufferToRgba32Buffer();

	
	if (m_cpuSkyTextureHandle == 0)
	{
		Texture2dConfigDX11 tex2dConfig;
		tex2dConfig.SetWidth(m_windowWidth);
		tex2dConfig.SetHeight(m_windowHeight);
		tex2dConfig.SetFormat(DXGI_FORMAT_R8G8B8A8_UNORM);
		tex2dConfig.SetMipLevels(1);
		tex2dConfig.SetUsage(D3D11_USAGE_DYNAMIC);
		tex2dConfig.SetCPUAccessFlags(D3D11_CPU_ACCESS_WRITE);
		D3D11_SUBRESOURCE_DATA subResource;
		subResource.pSysMem = &m_rgba32Buffer[0];
		subResource.SysMemPitch = m_windowWidth * 4;
		subResource.SysMemSlicePitch = 0;
		m_cpuSkyTextureHandle = TextureDX11ResourceFactory::getInstance().createTexture2D("CpuSkyTexture", tex2dConfig, &subResource);
		return;
	}

	Texture2dDX11* srcTex2d = (Texture2dDX11*)TextureDX11ResourceFactory::getInstance().getResource(m_cpuSkyTextureHandle);

	D3D11_MAPPED_SUBRESOURCE mappedSubResource;
	//mappedSubResource.pData = &m_rgba32Buffer[0];
	//mappedSubResource.RowPitch = m_windowWidth * 4;
	//mappedSubResource.DepthPitch = 0;
	HRESULT hr = RendererDX11::getInstance().getDeviceContex()->Map(srcTex2d->getTexture2D(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource);

	memcpy(mappedSubResource.pData, &m_rgba32Buffer[0], m_rgba32Buffer.size() * sizeof(uint32_t));

	RendererDX11::getInstance().getDeviceContex()->Unmap(srcTex2d->getTexture2D(), 0); 
}

void SkyRenderCPUTest::rgbaFloatBufferToRgba32Buffer()
{
	if (m_rgba32Buffer.size() != m_windowWidth * m_windowHeight)
		m_rgba32Buffer.resize(m_windowHeight * m_windowWidth);

	for (size_t i = 0; i < m_radianceRGBBuffer.size(); ++i)
	{
		uint32_t r = m_radianceRGBBuffer[i].m_x * 255;
		uint32_t g = m_radianceRGBBuffer[i].m_y * 255;
		uint32_t b = m_radianceRGBBuffer[i].m_z * 255;
		uint32_t a = 255;

		//下面这个是png的顺序
		//uint32_t irgb = a << 24;
		//irgb += r << 16;
		//irgb += g << 8;
		//irgb += b;
		//这个是dx rgba的顺序
		uint32_t irgb = a << 24;
		irgb += b << 16;
		irgb += g << 8;
		irgb += r;

		m_rgba32Buffer[i] = irgb;
	}
}

void SkyRenderCPUTest::saveRadianceRGBBufferToFile()
{
	/*uint32_t* iRGBBuffer = new uint32_t[m_radianceRGBBuffer.size()];

	for (size_t i = 0; i < m_radianceRGBBuffer.size(); ++i)
	{
		uint32_t r = m_radianceRGBBuffer[i].m_x * 255;
		uint32_t g = m_radianceRGBBuffer[i].m_y * 255;
		uint32_t b = m_radianceRGBBuffer[i].m_z * 255;
		uint32_t a = 255;

		uint32_t irgb = a << 24;
		irgb += r << 16;
		irgb += g << 8;
		irgb += b;

		iRGBBuffer[i] = irgb;
	}*/

	rgbaFloatBufferToRgba32Buffer();

	//write_png("c:\\out.png", &m_rgba32Buffer[0], m_windowWidth, m_windowHeight);

	//SAFE_DELETE_ARRAY(iRGBBuffer);
}

//void SkyRenderCPUTest::onGUI()
//{
//	static bool g_overLayShow = true;
//	ImGui::SetNextWindowPos(ImVec2(100, 300));
//	if (!ImGui::Begin("SkyRender", &g_overLayShow, ImVec2(500, 800), 0.7f, ImGuiWindowFlags_NoTitleBar))
//	{
//		ImGui::End();
//		return;
//	}
//
//	//if(ImGui::Button("CPU Render", ImVec2(200, 50)))
//	//{
//	//	m_isProcessing = true;
//	//	renderSky();
//	//}
//
//	//ImGui::SliderFloat("sun theta", &m_sunTheta, 0.0f, 180);  
//
//	//ImGui::SliderFloat("sun phi", &m_sunPhi, 0.0f, 360.0f);
//
//	if (m_isProcessing)
//	{
//		float fProgress = (float)g_atomicProgress / (m_windowHeight * m_windowWidth);
//		ImGui::ProgressBar(fProgress, ImVec2(150, 20));
//		if (fProgress >= 1.0f)
//		{
//			m_isProcessing = false;
//
//			updateCpuSkyTexture();
//		}			
//	}
//	
//	if (m_cpuSkyTextureHandle != 0)
//	{
//		Texture2dDX11* srcTex2d = (Texture2dDX11*)TextureDX11ResourceFactory::getInstance().getResource(m_cpuSkyTextureHandle);
//
//		ImGui::Image(srcTex2d->getShaderResourceView(), ImVec2(200, 200));
//	}
//
//	
//	ImGui::End();
//}

Vector3	SkyRenderCPUTest::getVec3SolarIrradiance()
{
	double r = m_atmosphereParameters.solar_irradiance(kLambdaR).to(watt_per_square_meter_per_nm);
	double g = m_atmosphereParameters.solar_irradiance(kLambdaG).to(watt_per_square_meter_per_nm);
	double b = m_atmosphereParameters.solar_irradiance(kLambdaB).to(watt_per_square_meter_per_nm);
	return Vector3(r, g, b);
}

Vector3	SkyRenderCPUTest::getVec3MieScattering()
{
	double r = m_atmosphereParameters.mie_scattering(kLambdaR).to(nm);
	double g = m_atmosphereParameters.mie_scattering(kLambdaG).to(watt_per_square_meter_per_nm);
	double b = m_atmosphereParameters.mie_scattering(kLambdaB).to(watt_per_square_meter_per_nm);
	return Vector3(r, g, b);
}

NAME_SPACE_END
NAME_SPACE_END
NAME_SPACE_END