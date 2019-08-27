/*
测试大气模型，现在只做cpu端
*/
#include "stdafx.h"
#include <array>
#include "Environment/Atmosphere/Reference/Model.h"
#include "Environment/Atmosphere/Reference/Definitions.h"
#include "Environment/Atmosphere/Model.h"
#include "Dimensional/test_case.h"
#include "SDK/minpng.h"

NAME_SPACE_BEGIN_APOLLO
NAME_SPACE_BEGIN_ATMOSPHERE
NAME_SPACE_BEGIN_REFERENCE

/*
我们的测试场景是一个球形星球上有一个球。 它的位置和大小由以下常量指定（请注意，我们使用大球体，以便它可以产生可见光轴，以便测试它们）：
*/
constexpr Length kSphereRadius = 1.0 * km;
constexpr Position kSphereCenter = Position(0.0 * km, 0.0 * km, kSphereRadius);

//我们的测试使用以公里数表示的长度值，对于基于辐射值的测试（相对亮度值），使用以下3个波长：
constexpr Length kLengthUnit = 1.0 * km;
constexpr Wavelength kLambdaR = Apollo::Atmosphere::Model::kLambdaR * nm;
constexpr Wavelength kLambdaG = Apollo::Atmosphere::Model::Model::kLambdaG * nm;
constexpr Wavelength kLambdaB = Apollo::Atmosphere::Model::Model::kLambdaB * nm;

typedef std::unique_ptr<unsigned int[]> Image;

const char kOutputDir[] = "output/Doc/atmosphere/reference/";
constexpr unsigned int kWidth = 640;
constexpr unsigned int kHeight = 360;

void WritePngArgb(const std::string& name, void* pixels) 
{
	//write_png((std::string(kOutputDir) + name).c_str(), pixels, kWidth, kHeight);
}

using std::max;
using std::min;

class ModelTest : public dimensional::TestCase 
{
public:
	template<typename T>
	ModelTest(const std::string& name, T test) : TestCase("ModelTest " + name, static_cast<Test>(test)), name_(name) {}

	/*
	在每个测试用例之前调用SetUp方法。 
	我们在这里放置了必须在任何测试用例之前执行的初始化代码，
	即大气参数的初始化（我们所有的测试使用相同的大气参数）和恒定的场景参数（地球中心，太阳大小和辐射，表面反照率）：
	*/
	void SetUp() override 
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
		constexpr NumberDensity kMaxOzoneNumberDensity = 300.0 * kDobsonUnit / (15.0 * km);

		std::vector<SpectralIrradiance> solar_irradiance;
		std::vector<ScatteringCoefficient> rayleigh_scattering;
		std::vector<ScatteringCoefficient> mie_scattering;
		std::vector<ScatteringCoefficient> mie_extinction;
		std::vector<ScatteringCoefficient> absorption_extinction;
		for (int l = kLambdaMin; l <= kLambdaMax; l += 10) 
		{
			double lambda = static_cast<double>(l) * 1e-3;  // micro-meters
			SpectralIrradiance solar = kSolarIrradiance[(l - kLambdaMin) / 10] *
				watt_per_square_meter_per_nm;
			float test = pow(lambda, -4);
			ScatteringCoefficient rayleigh = kRayleigh * pow(lambda, -4);
			ScatteringCoefficient mie = kMieAngstromBeta / kMieScaleHeight *
				pow(lambda, -kMieAngstromAlpha);
			solar_irradiance.push_back(solar);
			rayleigh_scattering.push_back(rayleigh);
			mie_scattering.push_back(mie * kMieSingleScatteringAlbedo);
			mie_extinction.push_back(mie);
			absorption_extinction.push_back(kMaxOzoneNumberDensity *
				kOzoneCrossSection[(l - kLambdaMin) / 10] * m2);
		}

		atmosphere_parameters_.solar_irradiance = IrradianceSpectrum(
			kLambdaMin * nm, kLambdaMax * nm, solar_irradiance);
		atmosphere_parameters_.sun_angular_radius = 0.2678 * deg;
		atmosphere_parameters_.bottom_radius = 6360.0 * km;
		atmosphere_parameters_.top_radius = 6420.0 * km;
		atmosphere_parameters_.rayleigh_density.layers[1] = DensityProfileLayer(
			0.0 * m, 1.0, -1.0 / kRayleighScaleHeight, 0.0 / m, 0.0);
		atmosphere_parameters_.rayleigh_scattering = ScatteringSpectrum(
			kLambdaMin * nm, kLambdaMax * nm, rayleigh_scattering);
		atmosphere_parameters_.mie_density.layers[1] = DensityProfileLayer(
			0.0 * m, 1.0, -1.0 / kMieScaleHeight, 0.0 / m, 0.0);
		atmosphere_parameters_.mie_scattering = ScatteringSpectrum(
			kLambdaMin * nm, kLambdaMax * nm, mie_scattering);
		atmosphere_parameters_.mie_extinction = ScatteringSpectrum(
			kLambdaMin * nm, kLambdaMax * nm, mie_extinction);
		atmosphere_parameters_.mie_phase_function_g = kMiePhaseFunctionG;
		// Density profile increasing linearly from 0 to 1 between 10 and 25km, and
		// decreasing linearly from 1 to 0 between 25 and 40km. Approximate profile
		// from http://www.kln.ac.lk/science/Chemistry/Teaching_Resources/Documents/
		// Introduction%20to%20atmospheric%20chemistry.pdf (page 10).
		atmosphere_parameters_.absorption_density.layers[0] = DensityProfileLayer(
			25.0 * km, 0.0, 0.0 / km, 1.0 / (15.0 * km), -2.0 / 3.0);
		atmosphere_parameters_.absorption_density.layers[1] = DensityProfileLayer(
			0.0 * km, 0.0, 0.0 / km, -1.0 / (15.0 * km), 8.0 / 3.0);
		atmosphere_parameters_.absorption_extinction = ScatteringSpectrum(
			kLambdaMin * nm, kLambdaMax * nm, absorption_extinction);
		atmosphere_parameters_.ground_albedo = DimensionlessSpectrum(0.1);
		atmosphere_parameters_.mu_s_min = cos(102.0 * deg);

		earth_center_ =
			Position(0.0 * m, 0.0 * m, -atmosphere_parameters_.bottom_radius);

		sun_size_ = dimensional::vec2(
			tan(atmosphere_parameters_.sun_angular_radius),
			cos(atmosphere_parameters_.sun_angular_radius));

		ground_albedo_ = GetGrassAlbedo();
		sphere_albedo_ = GetSnowAlbedo();

		double ground_albedo_R = ground_albedo_(kLambdaR).to(1);
		double ground_albedo_G = ground_albedo_(kLambdaG).to(1);
		double ground_albedo_B = ground_albedo_(kLambdaB).to(1);

		double sphere_albedo_R = sphere_albedo_(kLambdaR).to(1);
		double sphere_albedo_G = sphere_albedo_(kLambdaG).to(1);
		double sphere_albedo_B = sphere_albedo_(kLambdaB).to(1);

		//for rgb
		double solar_R = atmosphere_parameters_.solar_irradiance(kLambdaR).to(watt_per_square_meter_per_nm);
		double solar_G = atmosphere_parameters_.solar_irradiance(kLambdaG).to(watt_per_square_meter_per_nm);
		double solar_B = atmosphere_parameters_.solar_irradiance(kLambdaB).to(watt_per_square_meter_per_nm);

		double rayLeighScatter_R = atmosphere_parameters_.rayleigh_scattering(kLambdaR).to(1.0/m);
		double rayLeighScatter_G = atmosphere_parameters_.rayleigh_scattering(kLambdaG).to(1.0 / m);
		double rayLeighScatter_B = atmosphere_parameters_.rayleigh_scattering(kLambdaB).to(1.0 / m);

		double mieScattering_R = atmosphere_parameters_.mie_scattering(kLambdaR).to(1.0 / m);
		double mieScattering_G = atmosphere_parameters_.mie_scattering(kLambdaG).to(1.0 / m);
		double mieScattering_B = atmosphere_parameters_.mie_scattering(kLambdaB).to(1.0 / m);

		double mieExtinction_R = atmosphere_parameters_.mie_extinction(kLambdaR).to(1.0 / m);
		double mieExtinction_G = atmosphere_parameters_.mie_extinction(kLambdaG).to(1.0 / m);
		double mieExtinction_B = atmosphere_parameters_.mie_extinction(kLambdaB).to(1.0 / m);

		double absorptionExtinction_R = atmosphere_parameters_.absorption_extinction(kLambdaR).to(1.0 / m);
		double absorptionExtinction_G = atmosphere_parameters_.absorption_extinction(kLambdaG).to(1.0 / m);
		double absorptionExtinction_B = atmosphere_parameters_.absorption_extinction(kLambdaB).to(1.0 / m);
	}

	//地面和球的albedo
	DimensionlessSpectrum GetGrassAlbedo() 
	{
		// Grass spectral albedo from Uwe Feister and Rolf Grewe, "Spectral albedo
		// measurements in the UV and visible region over different types of
		// surfaces", Photochemistry and Photobiology, 62, 736-744, 1995.
		constexpr double kGrassAlbedo[45] = {
			0.018, 0.019, 0.019, 0.020, 0.022, 0.024, 0.027, 0.029, 0.030, 0.031,
			0.032, 0.032, 0.032, 0.033, 0.035, 0.040, 0.055, 0.073, 0.084, 0.089,
			0.089, 0.079, 0.069, 0.063, 0.061, 0.057, 0.052, 0.051, 0.048, 0.042,
			0.039, 0.035, 0.035, 0.043, 0.087, 0.156, 0.234, 0.334, 0.437, 0.513,
			0.553, 0.571, 0.579, 0.581, 0.587
		};
		std::vector<Number> grass_albedo_samples;
		for (int i = 0; i < 45; ++i) {
			grass_albedo_samples.push_back(kGrassAlbedo[i]);
		}
		return DimensionlessSpectrum(360.0 * nm, 800.0 * nm, grass_albedo_samples);
	}

	DimensionlessSpectrum GetSnowAlbedo()
	{
		// Snow 5cm spectral albedo from Uwe Feister and Rolf Grewe, "Spectral
		// albedo measurements in the UV and visible region over different types of
		// surfaces", Photochemistry and Photobiology, 62, 736-744, 1995.
		constexpr double kSnowAlbedo[7] = {
			0.796, 0.802, 0.807, 0.810, 0.818, 0.825, 0.826
		};
		std::vector<Number> snow_albedo_samples;
		for (int i = 0; i < 7; ++i) {
			snow_albedo_samples.push_back(kSnowAlbedo[i]);
		}
		return DimensionlessSpectrum(360.0 * nm, 420.0 * nm, snow_albedo_samples);
	}

	//GPU模型根据测试用例进行了不同的初始化，因此我们提供了一个单独的方法来初始化它：
	void InitGpuModel(bool combine_textures, bool precomputed_luminance)
	{
	}

	//同样，所有测试用例可能都不需要CPU模型，因此我们提供了一个单独的方法来初始化它：
	void InitCpuModel() 
	{
		reference_model_.reset(
			new Apollo::Atmosphere::Reference::Model(atmosphere_parameters_, "h:\\"));
		reference_model_->Init();
	}

	//最后，在使用GPU或CPU模型渲染图像之前，我们必须初始化相机（位置，变换矩阵，曝光）和太阳方向，并选择渲染输出（辐射或亮度）。 
	//为此，我们提供以下方法：
	void SetViewParameters(Angle sun_theta, Angle sun_phi, bool use_luminance) 
	{
		// Transform matrix from camera frame to world space (i.e. the inverse of a
		// GL_MODELVIEW matrix).
		const float kCameraPos[3] = { 2000.0, -8000.0, 500.0 };
		constexpr float kPitch = PI / 30.0;
		const float model_from_view[16] = {
			1.0, 0.0, 0.0, kCameraPos[0],
			0.0, -sinf(kPitch), -cosf(kPitch), kCameraPos[1],
			0.0, cosf(kPitch), -sinf(kPitch), kCameraPos[2],
			0.0, 0.0, 0.0, 1.0
		};

		// Transform matrix from clip space to camera space (i.e. the inverse of a
		// GL_PROJECTION matrix).
		constexpr float kFovY = 50.0 / 180.0 * PI;
		const float kTanFovY = std::tan(kFovY / 2.0);
		const float view_from_clip[16] = {
			kTanFovY * static_cast<float>(kWidth) / kHeight, 0.0, 0.0, 0.0,
			0.0, kTanFovY, 0.0, 0.0,
			0.0, 0.0, 0.0, -1.0,
			0.0, 0.0, 1.0, 1.0
		};

		// Transform matrix from clip space to world space.
		for (int row = 0; row < 3; ++row) {
			for (int col = 0; col < 3; ++col) {
				int col2 = col < 2 ? col : 3;
				model_from_clip_[col + 3 * row] =
					model_from_view[0 + 4 * row] * view_from_clip[col2 + 0] +
					model_from_view[1 + 4 * row] * view_from_clip[col2 + 4] +
					model_from_view[2 + 4 * row] * view_from_clip[col2 + 8];
			}
		}

		camera_ = Position(kCameraPos[0] * m, kCameraPos[1] * m, kCameraPos[2] * m);
		exposure_ = use_luminance ? 1e-4 : 10.0;
		use_luminance_ = use_luminance;
		sun_direction_ = Direction(
			cos(sun_phi) * sin(sun_theta),
			sin(sun_phi) * sin(sun_theta),
			cos(sun_theta));
	}

	//最后一个“setup”方法是TearDown方法，它在每个测试用例之后调用。 它释放了测试期间可能已分配的所有资源：
	void TearDown() override
	{
		model_ = nullptr;
		reference_model_ = nullptr;
		//if (program_) {
		//	glDeleteProgram(program_);
		//}
	}

	/*
	渲染方法

	一旦使用上述方法初始化测试夹具，我们就可以使用GPU或CPU模型渲染一个或多个图像。 
	对于GPU模型，我们必须首先初始化GPU着色器，这可以使用以下方法完成：
	*/
	void InitShader()
	{
	}

	//借助此方法，我们现在可以实现一种使用GPU模型渲染图像的方法。 
	//为此，我们只需要使用GPU程序渲染全屏四边形，然后回读帧缓冲像素。
	Image RenderGpuImage()
	{
		InitShader();

		//todo
	}

	/*
	为了使用CPU模型渲染图像，我们必须首先提供其CPU实现。 
	为此，对于GPU模型的单元测试，我们只是将GLSL着色器model_test.glsl视为C ++代码（参见简介），
	我们直接在这里包含（在它需要的函数和宏的定义之后 - “ “制服”由测试夹具类的字段提供，在此文件的末尾定义）：
	*/
	RadianceSpectrum GetSolarRadiance() 
	{
		return reference_model_->GetSolarRadiance();
	}

	RadianceSpectrum GetSkyRadiance(Position camera, Direction view_ray,
		Length shadow_length, Direction sun_direction,
		DimensionlessSpectrum& transmittance)
	{
		return reference_model_->GetSkyRadiance(
			camera, view_ray, shadow_length, sun_direction, &transmittance);
	}

	RadianceSpectrum GetSkyRadianceToPoint(Position camera, Position point,
		Length shadow_length, Direction sun_direction,
		DimensionlessSpectrum& transmittance)
	{
		return reference_model_->GetSkyRadianceToPoint(
			camera, point, shadow_length, sun_direction, &transmittance);
	}

	IrradianceSpectrum GetSunAndSkyIrradiance(Position point, Direction normal,
		Direction sun_direction, IrradianceSpectrum& sky_irradiance)
	{
		return reference_model_->GetSunAndSkyIrradiance(
			point, normal, sun_direction, &sky_irradiance);
	}

#define _OUT(x) x&
#include "../bin/Assets/Shader/Model_Test.hlsl"

	/*
	通过这种CPU实现，我们可以在所有像素上使用简单循环渲染图像，为每个像素调用GetViewRayRadiance，
	并使用与GPU版本相同的色调映射函数将结果转换为最终颜色。 
	与GPU模型的主要区别在于从辐射光谱到sRGB值的转换，
	如果需要亮度输出，必须明确地完成（否则，对于辐射输出，我们只需要在3个预定波长处采样辐射光谱））：
	*/
	Image RenderCpuImage() 
	{
		constexpr auto kMaxLuminousEfficacy = MAX_LUMINOUS_EFFICACY * lm / watt;
		std::vector<Wavelength> wavelengths;
		std::vector<Number> x_values;
		std::vector<Number> y_values;
		std::vector<Number> z_values;
		for (unsigned int i = 0; i < 95 * 4; i += 4) 
		{
			wavelengths.push_back(CIE_2_DEG_COLOR_MATCHING_FUNCTIONS[i] * nm);
			x_values.push_back(CIE_2_DEG_COLOR_MATCHING_FUNCTIONS[i + 1]);
			y_values.push_back(CIE_2_DEG_COLOR_MATCHING_FUNCTIONS[i + 2]);
			z_values.push_back(CIE_2_DEG_COLOR_MATCHING_FUNCTIONS[i + 3]);
		}
		const auto cie_x_bar = DimensionlessSpectrum(wavelengths, x_values);
		const auto cie_y_bar = DimensionlessSpectrum(wavelengths, y_values);
		const auto cie_z_bar = DimensionlessSpectrum(wavelengths, z_values);

		Image pixels(new unsigned int[kWidth * kHeight]);
		//ProgressBar progress_bar(kWidth * kHeight);
		for (unsigned int j = 0; j < kHeight; ++j) 
		{
			double y = 1.0 - 2.0 * (j + 0.5) / kHeight;
			double dy = -2.0 / kHeight;
			for (unsigned int i = 0; i < kWidth; ++i)
			{
				double x = 2.0 * (i + 0.5) / kWidth - 1.0;
				double dx = 2.0 / kWidth;

				Direction view_ray(
					model_from_clip_[0] * x + model_from_clip_[1] * y +
					model_from_clip_[2],
					model_from_clip_[3] * x + model_from_clip_[4] * y +
					model_from_clip_[5],
					model_from_clip_[6] * x + model_from_clip_[7] * y +
					model_from_clip_[8]);

				Direction view_ray_diff(
					model_from_clip_[0] * dx + model_from_clip_[1] * dy,
					model_from_clip_[3] * dx + model_from_clip_[4] * dy,
					model_from_clip_[6] * dx + model_from_clip_[7] * dy);

				RadianceSpectrum radiance = GetViewRayRadiance(view_ray, view_ray_diff);

				double r, g, b;
				if (use_luminance_) 
				{
					Luminance x = kMaxLuminousEfficacy * Integral(radiance * cie_x_bar);
					Luminance y = kMaxLuminousEfficacy * Integral(radiance * cie_y_bar);
					Luminance z = kMaxLuminousEfficacy * Integral(radiance * cie_z_bar);
					r = (XYZ_TO_SRGB[0] * x + XYZ_TO_SRGB[1] * y + XYZ_TO_SRGB[2] * z).to(
						cd_per_square_meter);
					g = (XYZ_TO_SRGB[3] * x + XYZ_TO_SRGB[4] * y + XYZ_TO_SRGB[5] * z).to(
						cd_per_square_meter);
					b = (XYZ_TO_SRGB[6] * x + XYZ_TO_SRGB[7] * y + XYZ_TO_SRGB[8] * z).to(
						cd_per_square_meter);
				}
				else 
				{
					r = radiance(kLambdaR).to(watt_per_square_meter_per_sr_per_nm);
					g = radiance(kLambdaG).to(watt_per_square_meter_per_sr_per_nm);
					b = radiance(kLambdaB).to(watt_per_square_meter_per_sr_per_nm);
				}

				r = std::pow(1.0 - std::exp(-r * exposure_()), 1.0 / 2.2);
				g = std::pow(1.0 - std::exp(-g * exposure_()), 1.0 / 2.2);
				b = std::pow(1.0 - std::exp(-b * exposure_()), 1.0 / 2.2);
				unsigned int red = static_cast<unsigned int>(r * 255.0);
				unsigned int green = static_cast<unsigned int>(g * 255.0);
				unsigned int blue = static_cast<unsigned int>(b * 255.0);
				pixels[i + j * kWidth] = (255 << 24) | (red << 16) | (green << 8) | blue;
				//progress_bar.Increment(1);
			}
		}
		write_png("h:\\model_test.png", pixels.get(), kWidth, kHeight);
		return pixels;
	}

	/*
	比较方法

	在渲染了一些图像之后，我们想要比较它们，以便检查用不同方法渲染的同一场景的两个图像是否足够接近。 
	这是以下方法的目标，其使用峰值信噪比作为图像差异度量。
	*/
	double ComputePSNR(const unsigned int* image1, const unsigned int* image2) 
	{
		double square_error_sum = 0.0;
		for (unsigned int j = 0; j < kHeight; ++j) 
		{
			for (unsigned int i = 0; i < kWidth; ++i) 
			{
				int argb1 = image1[i + j * kWidth];
				int argb2 = image2[i + j * kWidth];
				dimensional::Vector3<Number> rgb1(
					(argb1 >> 16) & 0xFF, (argb1 >> 8) & 0xFF, argb1 & 0xFF);
				dimensional::Vector3<Number> rgb2(
					(argb2 >> 16) & 0xFF, (argb2 >> 8) & 0xFF, argb2 & 0xFF);
				square_error_sum += dot(rgb1 - rgb2, rgb1 - rgb2)();
			}
		}
		double mean_square_error = sqrt(square_error_sum / (kWidth * kHeight));
		return 10.0 * log(255 * 255 / mean_square_error) / log(10.0);
	}

	/*
	此外，为了在视觉上比较图像，有一个HTML测试报告是有用的，为每个测试用例显示它的两个图像和它们的PSNR得分差异。
	为此，以下方法比较两个映像，将它们写入磁盘，在测试报告文件中创建或附加测试报告条目，最后返回计算的PSNR。
	*/
	double Compare(Image image1, Image image2, const std::string& caption,bool append) 
	{
		double psnr = ComputePSNR(image1.get(), image2.get());
		WritePngArgb(name_ + "1.png", image1.get());
		WritePngArgb(name_ + "2.png", image2.get());
		std::ofstream file(std::string(kOutputDir) + "test_report.html",
			append ? std::ios_base::app : std::ios_base::trunc);
		file << "<h2>" << name_ << " (PSNR = " << psnr << "dB)</h2>" << std::endl
			<< "<p>" << caption << std::endl
			<< "<p><img src=\"" << name_ << "1.png\">" << std::endl
			<< "<img src=\"" << name_ << "2.png\">" << std::endl;
		file.close();
		return psnr;
	}

	/*
	测试用例

	由于上面的辅助方法，我们最终可以实现一些测试用例，以确保GPU模型尽管有近似值，但几乎与完整光谱一样精确，但CPU模型要慢得多。

	第一个测试用例比较了在GPU和CPU上完成的辐射计算。 
	它使用单独的RGB纹理进行单个Mie散射，这意味着GPU和CPU的3个波长的计算完全相同（浮点精度除外：GPU上的单精度） -
	预计算纹理中的半精度，而CPU上的双精度）。 因此，我们希望这两个图像几乎相同：
	*/
	void TestRadianceSeparateTextures() 
	{
		const std::string kCaption = "Left: GPU model, combine_textures = false. "
			"Right: CPU model. Both images show the spectral radiance at 3 "
			"predefined wavelengths (i.e. no conversion to sRGB via CIE XYZ).";
		//InitGpuModel(false /* combine_textures */,
		//	false /* precomputed_luminance */);
		InitCpuModel();
		SetViewParameters(65.0 * deg, 90.0 * deg, false /* use_luminance */);
		RenderCpuImage();
		//ExpectLess(47.0, Compare(RenderGpuImage(), RenderCpuImage(), kCaption, false));
	}

	/*
	以下测试用例与前一个测试用例几乎相同，只是我们使用GPU模型中的combine_textures选项。 
	这导致GPU侧的一些近似值，而不是CPU模型中存在的近似值。
	因此，与之前的测试案例相比，我们预计两幅图像之间的差异会略大：
	*/
	void TestRadianceCombineTextures() 
	{
		const std::string kCaption = "Left: GPU model, combine_textures = true. "
			"Right: CPU model. Both images show the spectral radiance at 3 "
			"predefined wavelengths (i.e. no conversion to sRGB via CIE XYZ).";
		InitGpuModel(true /* combine_textures */,
			false /* precomputed_luminance */);
		InitCpuModel();
		SetViewParameters(65.0 * deg, 90.0 * deg, false /* use_luminance */);
		ExpectLess(
			46.0, Compare(RenderGpuImage(), RenderCpuImage(), kCaption, true));
	}

	/*
	对于日落场景，以下测试用例与前一测试用例相同。 
	在这种情况下，单个Mie散射贡献大于之前的测试，因此我们期望GPU和CPU结果之间的差异更大（由于单个Mie散射项的GPU近似）：
	*/
	void TestRadianceCombineTexturesSunSet()
	{
		const std::string kCaption = "Left: GPU model, combine_textures = true. "
			"Right: CPU model. Both images show the spectral radiance at 3 "
			"predefined wavelengths (i.e. no conversion to sRGB via CIE XYZ).";
		InitGpuModel(true /* combine_textures */,
			false /* precomputed_luminance */);
		InitCpuModel();
		SetViewParameters(88.0 * deg, 90.0 * deg, false /* use_luminance */);
		RenderCpuImage();
	//	ExpectLess(40.0, Compare(RenderGpuImage(), RenderCpuImage(), kCaption, true));
	}

private:
	std::string name_;
	AtmosphereParameters atmosphere_parameters_;
	DimensionlessSpectrum ground_albedo_;
	DimensionlessSpectrum sphere_albedo_;
	Position earth_center_;
	dimensional::vec2 sun_size_;

	std::unique_ptr<Apollo::Atmosphere::Model> model_;
	std::unique_ptr<Apollo::Atmosphere::Reference::Model> reference_model_;
	
	std::array<float, 9> model_from_clip_;
	Position camera_;
	Number exposure_;
	bool use_luminance_;
	Direction sun_direction_;
};

//ModelTest radiance1(
//	"RadianceSeparateTextures",
//	&ModelTest::TestRadianceSeparateTextures);
//
//ModelTest radiance2(
//	"RadianceCombineTextures",
//	&ModelTest::TestRadianceCombineTextures);

ModelTest radiance3(
	"RadianceCombineTexturesSunSet",
	&ModelTest::TestRadianceCombineTexturesSunSet);

NAME_SPACE_END
NAME_SPACE_END
NAME_SPACE_END