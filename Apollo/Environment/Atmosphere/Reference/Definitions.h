#pragma once

/*
这个头文件定义用在SkyFuntion.hlsl里的大气层模型的物理类型和常量，可以被c++编译。
HLSL等价文件SkyDefinitions.hlsl提供一样的类型和常量，允许一样的函数被HLSL编译器编译。

这个C ++编译的主要目的是检查HLSL表达式的维度同质性( dimensional homogeneity)。
为此，我们使用通过物理维度参数化的通用模板来定义C ++物理类型，这些模板受Boost.Unit的启发，并由以下包含的文件提供：
*/
#include "Environment/Atmosphere/Constants.h"
#include "Dimensional/angle.h"
#include "Dimensional/binary_function.h"
#include "Dimensional/scalar.h"
#include "Dimensional/scalar_function.h"
#include "Dimensional/ternary_function.h"
#include "Dimensional/vector.h"

//namespace Apollo
//{
//	namespace Atmosphere
//	{
//		namespace Reference
//		{
//			/*
//			物理量
//			我们的大气模型所需的物理量是辐射度(radiometric)和光度量(photometric)。
//			我们从六个基本量开始：角度(angle)，长度(length)，波长(wavelength)，立体角(soild angle)，功率(power)和发光功率(luminous power)
//			（波长也是一个长度，但我们区分两者以提高清晰度）。
//			*/
//			typedef dimensional::Angle Aangle;
//			typedef dimensional::Scalar<1, 0, 0, 0, 0> Length;
//			typedef dimensional::Scalar<0, 1, 0, 0, 0> Wavelength;
//			typedef dimensional::Scalar<0, 0, 1, 0, 0> SolidAngle;
//			typedef dimensional::Scalar<0, 0, 0, 1, 0> Power;
//			typedef dimensional::Scalar<0, 0, 0, 0, 1> LuminousPower;
//
//			/*
//			由此我们得出辐照度(irradiance)，辐射度(radiance)，光谱辐照度(spectral rradiance)，光谱辐射率(spectral radiance)，亮度(luminance)等，
//			以及纯数(pure number)，面积(area)，体积(volume)等。
//			*/
//			typedef dimensional::Scalar<0, 0, 0, 0, 0> Number;
//			typedef dimensional::Scalar<-1, 0, 0, 0, 0> InverseLength;
//			typedef dimensional::Scalar<2, 0, 0, 0, 0> Area;
//			typedef dimensional::Scalar<3, 0, 0, 0, 0> Volume;
//			typedef dimensional::Scalar<-2, 0, 0, 1, 0> Irradiance;
//			typedef dimensional::Scalar<-2, 0, -1, 1, 0> Radiance;
//			typedef dimensional::Scalar<0, -1, 0, 1, 0> SpectralPower;
//			typedef dimensional::Scalar<-2, -1, 0, 1, 0> SpectralIrradiance;
//			typedef dimensional::Scalar<-2, -1, -1, 1, 0> SpectralRadiance;
//			typedef dimensional::Scalar<-3, -1, -1, 1, 0> SpectralRadianceDensity;
//			typedef dimensional::Scalar<-1, 0, 0, 0, 0> ScatteringCoefficient;
//			typedef dimensional::Scalar<0, 0, -1, 0, 0> InverseSolidAngle;
//			typedef dimensional::Scalar<-3, 0, 0, 0, 0> NumberDensity;
//			typedef dimensional::Scalar<0, 0, -1, 0, 1> LuminousIntensity;
//			typedef dimensional::Scalar<-2, 0, -1, 0, 1> Luminance;
//			typedef dimensional::Scalar<-2, 0, 0, 0, 1> Illuminance;
//
//			/*
//			我们还需要物理量的矢量，主要是根据波长来表示函数。
//			在这种情况下，矢量元素对应于某些预定义波长的函数值。
//			在这里，我们使用47个预定义波长，均匀分布在360和830纳米之间：
//			*/
//			template<int U1, int U2, int U3, int U4, int U5>
//			using WavelengthFunction = dimensional::ScalarFunction<0, 1, 0, 0, 0, U1, U2, U3, U4, U5, 47, 360, 830>;
//
//			// 波长到数值的函数。(名字里面的Spectrum表示就是从波长来，下面的同理)
//			typedef WavelengthFunction<0, 0, 0, 0, 0> DimensionlessSpectrum;
//			// 波长到功率的函数
//			typedef WavelengthFunction<0, -1, 0, 1, 0> PowerSpectrum;
//			// 从波长到辐照度的函数
//			typedef WavelengthFunction<-2, -1, 0, 1, 0> IrradianceSpectrum;
//			// 波长到辐射率的函数
//			typedef WavelengthFunction<-2, -1, -1, 1, 0> RadianceSpectrum;
//			// 波长到辐射率密度的函数
//			typedef WavelengthFunction<-3, -1, -1, 1, 0> RadianceDensitySpectrum;
//			// 波长到散射系数的函数
//			typedef WavelengthFunction<-1, 0, 0, 0, 0> ScatteringSpectrum;
//
//			// 3D中一个点（3个长度值）
//			typedef dimensional::Vector3<Length> Position;
//			// 3D中一个单位方向向量（无单位值）
//			typedef dimensional::Vector3<Number> Direction;
//			// 3个亮度值的向量
//			typedef dimensional::Vector3<Luminance> Luminance3;
//			// 3个照度值的向量
//			typedef dimensional::Vector3<Illuminance> Illuminance3;
//
//			/*
//			最后，我们还需要在每个纹素中包含物理量的预计算纹理（纹理大小在constants.h中定义）：
//			*/
//			typedef dimensional::BinaryFunction<
//				TRANSMITTANCE_TEXTURE_WIDTH,
//				TRANSMITTANCE_TEXTURE_HEIGHT,
//				DimensionlessSpectrum> TransmittanceTexture;
//
//			template<class T>
//			using AbstractScatteringTexture = dimensional::TernaryFunction<
//				SCATTERING_TEXTURE_WIDTH,
//				SCATTERING_TEXTURE_HEIGHT,
//				SCATTERING_TEXTURE_DEPTH,
//				T>;
//
//			typedef AbstractScatteringTexture<IrradianceSpectrum>
//				ReducedScatteringTexture;
//
//			typedef AbstractScatteringTexture<RadianceSpectrum>
//				ScatteringTexture;
//
//			typedef AbstractScatteringTexture<RadianceDensitySpectrum>
//				ScatteringDensityTexture;
//
//			typedef dimensional::BinaryFunction<
//				IRRADIANCE_TEXTURE_WIDTH,
//				IRRADIANCE_TEXTURE_HEIGHT,
//				IrradianceSpectrum> IrradianceTexture;
//		}
//
//		/*
//		物理单位
//		然后我们可以定义基本物理量的单位：弧度（rad），米（m），纳米（nm），球面度（sr），瓦特（瓦特）和流明（lm）：
//		*/
//		constexpr dimensional::Angle rad = dimensional::rad;
//		constexpr Length m = Length::Unit();
//		constexpr Wavelength nm = Wavelength::Unit();
//		constexpr SolidAngle sr = SolidAngle::Unit();
//		constexpr Power watt = Power::Unit();
//		constexpr LuminousPower lm = LuminousPower::Unit();
//
//		/*
//		从中我们可以推导出某些派生物理量的单位，以及一些派生单位（degress deg，km km km，kilocandela kcd）：
//		*/
//		constexpr double PI_ = dimensional::PI_;
//		constexpr dimensional::Angle pi = dimensional::pi;
//		constexpr dimensional::Angle deg = dimensional::deg;
//		constexpr Length km = 1000.0 * m;
//		constexpr Area m2 = m * m;
//		constexpr Volume m3 = m * m * m;
//		constexpr Irradiance watt_per_square_meter = watt / m2;
//		constexpr Radiance watt_per_square_meter_per_sr = watt / (m2 * sr);
//		constexpr SpectralIrradiance watt_per_square_meter_per_nm = watt / (m2 * nm);
//		constexpr SpectralRadiance watt_per_square_meter_per_sr_per_nm = watt / (m2 * sr * nm);
//		constexpr SpectralRadianceDensity watt_per_cubic_meter_per_sr_per_nm = watt / (m3 * sr * nm);
//		constexpr LuminousIntensity cd = lm / sr;
//		constexpr LuminousIntensity kcd = 1000.0 * cd;
//		constexpr Luminance cd_per_square_meter = cd / m2;
//		constexpr Luminance kcd_per_square_meter = kcd / m2;
//
//		/*
//		大气层参数
//		使用上述类型，我们现在可以定义大气模型的参数。 我们从密度profiles的定义开始，这些参数取决于高度：
//		*/
//		//大气层layer宽度为width，密度定义为exp_term * exp(exp_scale * h) + linear_term * h + constant_term
//		//clamp到[0,1]，h是海拔
//		struct DensityProfileLayer
//		{
//			DensityProfileLayer() : DensityProfileLayer(0.0 * m, 0.0, 0.0 / m, 0.0 / m, 0.0) {}
//			DensityProfileLayer(Length width, Number exp_term, InverseLength exp_scale, InverseLength linear_term, Number constant_term)
//				: width(width), exp_term(exp_term), exp_scale(exp_scale),
//				linear_term(linear_term), constant_term(constant_term) {}
//
//			Length width;
//			Number exp_term;
//			InverseLength exp_scale;
//			InverseLength linear_term;
//			Number constant_term;
//		};
//
//		/*
//		一个大气层密度profile由几层相互组成（从下到上）
//		最后一层的宽度被忽略，它总是延申到顶部大气层边界
//		peofile值从0到1
//		*/
//		struct DensityProfile
//		{
//			DensityProfileLayer layers[2];
//		};
//
//		struct AtmosphereParameters
//		{
//			// 大气层顶部的太阳辐照度
//			IrradianceSpectrum solar_irradiance;
//			// 太阳角度半径。Warming:这是个近似值，只有当角度小于0.1弧度时有效。
//			Angle sun_angular_radius;
//			// 地心到大气层底部边界的距离,在这里是地球半径
//			Length bottom_radius;
//			// 地心到大气层顶部边界的距离
//			Length top_radius;
//			//空气分子的density profile
//			//从0（零密度）和1（最大密度）的关于高度的无量纲值函数。
//			DensityProfile rayleigh_density;
//			/*
//			在空子分子密度最大的地方的散射系数（一般在大气层的底部），是一个关于波长的函数。
//			在海拔h的散射系数 = rayleigh_scattering * rayleigh_density
//			*/
//			ScatteringSpectrum rayleigh_scattering;
//			//气溶胶的density profile
//			//从0（零密度）和1（最大密度）的关于高度的无量纲值函数。
//			DensityProfile mie_density;
//			/*
//			在气溶胶密度最大的地方的散射系数（一般在大气层的底部），是一个关于波长的函数。
//			在海拔h的散射系数 = mie_scattering * mie_density
//			*/
//			ScatteringSpectrum mie_scattering;
//			/*
//			在气溶胶密度最大的地方的消光系数（一般在大气层的底部），是一个关于波长的函数。
//			在海拔h的消光系数 = mie_extinction * mie_density
//			*/
//			ScatteringSpectrum mie_extinction;
//			//关于气溶胶Cornette-Shanks相位函数的不对称参数
//			Number mie_phase_function_g;
//			//空气分子（比如臭氧）吸收光的density profile
//			//从0（零密度）和1（最大密度）的关于高度的无量纲值函数
//			DensityProfile absorption_density;
//			/*
//			在空气分子密度最大的地方的消光系数（比如臭氧），是一个关于波长的函数。
//			在海拔h的消光系数 = absorption_extinction * absorption_density
//			*/
//			ScatteringSpectrum absorption_extinction;
//			/* 地表的平均albedo
//				DimensionlessSpectrum ground_albedo;
//			/*
//			必须预先计算大气散射的最大太阳天顶角的余弦
//			（为了获得最大精度，使用最小的太阳天顶角产生可忽略不计的天光辐射值。例如，对于地球情况，102度是一个不错的选择 - 产生mu_s_min = -0.2）。
//			*/
//			Number mu_s_min;
//		}
//	}
//}


