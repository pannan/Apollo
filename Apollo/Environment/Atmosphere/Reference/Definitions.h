#pragma once

/*
���ͷ�ļ���������SkyFuntion.hlsl��Ĵ�����ģ�͵��������ͺͳ��������Ա�c++���롣
HLSL�ȼ��ļ�SkyDefinitions.hlsl�ṩһ�������ͺͳ���������һ���ĺ�����HLSL���������롣

���C ++�������ҪĿ���Ǽ��HLSL���ʽ��ά��ͬ����( dimensional homogeneity)��
Ϊ�ˣ�����ʹ��ͨ������ά�Ȳ�������ͨ��ģ��������C ++�������ͣ���Щģ����Boost.Unit���������������°������ļ��ṩ��
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
//			������
//			���ǵĴ���ģ��������������Ƿ����(radiometric)�͹����(photometric)��
//			���Ǵ�������������ʼ���Ƕ�(angle)������(length)������(wavelength)�������(soild angle)������(power)�ͷ��⹦��(luminous power)
//			������Ҳ��һ�����ȣ�������������������������ȣ���
//			*/
//			typedef dimensional::Angle Aangle;
//			typedef dimensional::Scalar<1, 0, 0, 0, 0> Length;
//			typedef dimensional::Scalar<0, 1, 0, 0, 0> Wavelength;
//			typedef dimensional::Scalar<0, 0, 1, 0, 0> SolidAngle;
//			typedef dimensional::Scalar<0, 0, 0, 1, 0> Power;
//			typedef dimensional::Scalar<0, 0, 0, 0, 1> LuminousPower;
//
//			/*
//			�ɴ����ǵó����ն�(irradiance)�������(radiance)�����׷��ն�(spectral rradiance)�����׷�����(spectral radiance)������(luminance)�ȣ�
//			�Լ�����(pure number)�����(area)�����(volume)�ȡ�
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
//			���ǻ���Ҫ��������ʸ������Ҫ�Ǹ��ݲ�������ʾ������
//			����������£�ʸ��Ԫ�ض�Ӧ��ĳЩԤ���岨���ĺ���ֵ��
//			���������ʹ��47��Ԥ���岨�������ȷֲ���360��830����֮�䣺
//			*/
//			template<int U1, int U2, int U3, int U4, int U5>
//			using WavelengthFunction = dimensional::ScalarFunction<0, 1, 0, 0, 0, U1, U2, U3, U4, U5, 47, 360, 830>;
//
//			// ��������ֵ�ĺ�����(���������Spectrum��ʾ���ǴӲ������������ͬ��)
//			typedef WavelengthFunction<0, 0, 0, 0, 0> DimensionlessSpectrum;
//			// ���������ʵĺ���
//			typedef WavelengthFunction<0, -1, 0, 1, 0> PowerSpectrum;
//			// �Ӳ��������նȵĺ���
//			typedef WavelengthFunction<-2, -1, 0, 1, 0> IrradianceSpectrum;
//			// �����������ʵĺ���
//			typedef WavelengthFunction<-2, -1, -1, 1, 0> RadianceSpectrum;
//			// �������������ܶȵĺ���
//			typedef WavelengthFunction<-3, -1, -1, 1, 0> RadianceDensitySpectrum;
//			// ������ɢ��ϵ���ĺ���
//			typedef WavelengthFunction<-1, 0, 0, 0, 0> ScatteringSpectrum;
//
//			// 3D��һ���㣨3������ֵ��
//			typedef dimensional::Vector3<Length> Position;
//			// 3D��һ����λ�����������޵�λֵ��
//			typedef dimensional::Vector3<Number> Direction;
//			// 3������ֵ������
//			typedef dimensional::Vector3<Luminance> Luminance3;
//			// 3���ն�ֵ������
//			typedef dimensional::Vector3<Illuminance> Illuminance3;
//
//			/*
//			������ǻ���Ҫ��ÿ�������а�����������Ԥ�������������С��constants.h�ж��壩��
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
//		����λ
//		Ȼ�����ǿ��Զ�������������ĵ�λ�����ȣ�rad�����ף�m�������ף�nm��������ȣ�sr�������أ����أ���������lm����
//		*/
//		constexpr dimensional::Angle rad = dimensional::rad;
//		constexpr Length m = Length::Unit();
//		constexpr Wavelength nm = Wavelength::Unit();
//		constexpr SolidAngle sr = SolidAngle::Unit();
//		constexpr Power watt = Power::Unit();
//		constexpr LuminousPower lm = LuminousPower::Unit();
//
//		/*
//		�������ǿ����Ƶ���ĳЩ�����������ĵ�λ���Լ�һЩ������λ��degress deg��km km km��kilocandela kcd����
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
//		���������
//		ʹ���������ͣ��������ڿ��Զ������ģ�͵Ĳ����� ���Ǵ��ܶ�profiles�Ķ��忪ʼ����Щ����ȡ���ڸ߶ȣ�
//		*/
//		//������layer���Ϊwidth���ܶȶ���Ϊexp_term * exp(exp_scale * h) + linear_term * h + constant_term
//		//clamp��[0,1]��h�Ǻ���
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
//		һ���������ܶ�profile�ɼ����໥��ɣ����µ��ϣ�
//		���һ��Ŀ�ȱ����ԣ����������굽����������߽�
//		peofileֵ��0��1
//		*/
//		struct DensityProfile
//		{
//			DensityProfileLayer layers[2];
//		};
//
//		struct AtmosphereParameters
//		{
//			// �����㶥����̫�����ն�
//			IrradianceSpectrum solar_irradiance;
//			// ̫���ǶȰ뾶��Warming:���Ǹ�����ֵ��ֻ�е��Ƕ�С��0.1����ʱ��Ч��
//			Angle sun_angular_radius;
//			// ���ĵ�������ײ��߽�ľ���,�������ǵ���뾶
//			Length bottom_radius;
//			// ���ĵ������㶥���߽�ľ���
//			Length top_radius;
//			//�������ӵ�density profile
//			//��0�����ܶȣ���1������ܶȣ��Ĺ��ڸ߶ȵ�������ֵ������
//			DensityProfile rayleigh_density;
//			/*
//			�ڿ��ӷ����ܶ����ĵط���ɢ��ϵ����һ���ڴ�����ĵײ�������һ�����ڲ����ĺ�����
//			�ں���h��ɢ��ϵ�� = rayleigh_scattering * rayleigh_density
//			*/
//			ScatteringSpectrum rayleigh_scattering;
//			//���ܽ���density profile
//			//��0�����ܶȣ���1������ܶȣ��Ĺ��ڸ߶ȵ�������ֵ������
//			DensityProfile mie_density;
//			/*
//			�����ܽ��ܶ����ĵط���ɢ��ϵ����һ���ڴ�����ĵײ�������һ�����ڲ����ĺ�����
//			�ں���h��ɢ��ϵ�� = mie_scattering * mie_density
//			*/
//			ScatteringSpectrum mie_scattering;
//			/*
//			�����ܽ��ܶ����ĵط�������ϵ����һ���ڴ�����ĵײ�������һ�����ڲ����ĺ�����
//			�ں���h������ϵ�� = mie_extinction * mie_density
//			*/
//			ScatteringSpectrum mie_extinction;
//			//�������ܽ�Cornette-Shanks��λ�����Ĳ��ԳƲ���
//			Number mie_phase_function_g;
//			//�������ӣ�������������չ��density profile
//			//��0�����ܶȣ���1������ܶȣ��Ĺ��ڸ߶ȵ�������ֵ����
//			DensityProfile absorption_density;
//			/*
//			�ڿ��������ܶ����ĵط�������ϵ�����������������һ�����ڲ����ĺ�����
//			�ں���h������ϵ�� = absorption_extinction * absorption_density
//			*/
//			ScatteringSpectrum absorption_extinction;
//			/* �ر��ƽ��albedo
//				DimensionlessSpectrum ground_albedo;
//			/*
//			����Ԥ�ȼ������ɢ������̫���춥�ǵ�����
//			��Ϊ�˻����󾫶ȣ�ʹ����С��̫���춥�ǲ����ɺ��Բ��Ƶ�������ֵ�����磬���ڵ��������102����һ�������ѡ�� - ����mu_s_min = -0.2����
//			*/
//			Number mu_s_min;
//		}
//	}
//}


