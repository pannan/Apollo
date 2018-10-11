/*
���ļ�Ϊʵ�����ǵĴ���ģ�͵�GLSL�����ṩ��Ԫ���ԡ� ���ǴӴ���������һЩ�����⣩ֵ�Ķ��忪ʼ��
*/

#include "stdafx.h"
#include <limits>
#include <string>
#include "Environment/Atmosphere/Reference/Functions.h"
#include "Environment/Atmosphere/Reference/Definitions.h"
#include "Environment/Atmosphere/Constants.h"
#include "Dimensional/test_case.h"

NAME_SPACE_BEGIN_APOLLO
NAME_SPACE_BEGIN_ATMOSPHERE
NAME_SPACE_BEGIN_REFERENCE

constexpr double kEpsilon = 1e-3;
//̫�����ն�
constexpr SpectralIrradiance kSolarIrradiance = 123.0 * watt_per_square_meter_per_nm;
//����뾶
constexpr Length kBottomRadius = 1000.0 * km;
//�����㶥���뾶
constexpr Length kTopRadius = 1500.0 * km;
constexpr Length kScaleHeight = 60.0 * km;
constexpr Length kRayleighScaleHeight = 60.0 * km;
constexpr Length kMieScaleHeight = 30.0 * km;
constexpr ScatteringCoefficient kRayleighScattering = 0.001 / km;
constexpr ScatteringCoefficient kMieScattering = 0.0015 / km;
constexpr ScatteringCoefficient kMieExtinction = 0.002 / km;
constexpr Number kGroundAlbedo = 0.1;

/*
�ø�������������ĳ���뾶r���춥�͵�ƽ��֮��ĽǶȵ�����ֵ�� ���ǽ����������Ե�ƽ���Ϸ����·��Ĺ��ߡ�
*/
Number CosineOfHorizonZenithAngle(Length r)
{
	assert(r >= kBottomRadius);
	return -sqrt(1.0 - (kBottomRadius / r) * (kBottomRadius / r));
}

/*
ĳЩ��Ԫ������ҪԤ�ȼ����������Ϊ���룬������Ч��ԭ�����ǲ�ϣ��Ϊ��Ԥ�ȼ�����������
���ǵĽ���������ṩ�ӳټ���������������ǵ�һ�γ��Զ�ȡ����ʱ�������ص�����
������Ҫ�ĵ�һ���������ӳ�͸��������ֵ��ζ�š���δ���㡱����
*/
class LazyTransmittanceTexture : public dimensional::BinaryFunction<
	TRANSMITTANCE_TEXTURE_WIDTH,
	TRANSMITTANCE_TEXTURE_HEIGHT, 
	DimensionlessSpectrum>
{
public:
	explicit LazyTransmittanceTexture(const AtmosphereParameters& atmosphere_parameters)
		: BinaryFunction(DimensionlessSpectrum(-1.0)),
		atmosphere_parameters_(atmosphere_parameters)
	{}

	virtual const DimensionlessSpectrum& Get(int i, int j) const
	{
		int index = i + j * TRANSMITTANCE_TEXTURE_HEIGHT;
		if (value_[index][0]() < 0.0)
		{
			value_[index] = ComputeTransmittanceToTopAtmosphereBoundaryTexture(atmosphere_parameters_, float2(i + 0.5, j + 0.5));
		}
		return value_[index];
	}

	void Clear()
	{
		constexpr unsigned int n = TRANSMITTANCE_TEXTURE_WIDTH * TRANSMITTANCE_TEXTURE_HEIGHT;
		for (unsigned int i = 0; i < n; ++i)
		{
			value_[i] = DimensionlessSpectrum(-1.0);
		}
	}

private:
	const AtmosphereParameters& atmosphere_parameters_;
};

//����Ҳ��Ҫһ���ӳٵ���ɢ������
class LazySingleScatteringTexture : public dimensional::TernaryFunction<
	SCATTERING_TEXTURE_WIDTH,
	SCATTERING_TEXTURE_HEIGHT,
	SCATTERING_TEXTURE_DEPTH,
	IrradianceSpectrum> 
{
public:
	LazySingleScatteringTexture(
		const AtmosphereParameters& atmosphere_parameters,
		const TransmittanceTexture& transmittance_texture,
		bool rayleigh)
		: TernaryFunction(IrradianceSpectrum(-watt_per_square_meter_per_nm)),
		atmosphere_parameters_(atmosphere_parameters),
		transmittance_texture_(transmittance_texture),
		rayleigh_(rayleigh) {}

	virtual const IrradianceSpectrum& Get(int i, int j, int k) const 
	{
		int index = i + SCATTERING_TEXTURE_WIDTH * (j + SCATTERING_TEXTURE_HEIGHT * k);
		if (value_[index][0] < 0.0 * watt_per_square_meter_per_nm) 
		{
			IrradianceSpectrum rayleigh;
			IrradianceSpectrum mie;
			ComputeSingleScatteringTexture(atmosphere_parameters_,
				transmittance_texture_, float3(i + 0.5, j + 0.5, k + 0.5),
				rayleigh, mie);
			value_[index] = rayleigh_ ? rayleigh : mie;
		}
		return value_[index];
	}

private:
	const AtmosphereParameters& atmosphere_parameters_;
	const TransmittanceTexture& transmittance_texture_;
	const bool rayleigh_;
};

//�ӳٶ��ɢ����������1
class LazyScatteringDensityTexture : public dimensional::TernaryFunction<
	SCATTERING_TEXTURE_WIDTH,
	SCATTERING_TEXTURE_HEIGHT,
	SCATTERING_TEXTURE_DEPTH,
	RadianceDensitySpectrum> 
{
public:
	LazyScatteringDensityTexture(
		const AtmosphereParameters& atmosphere_parameters,
		const TransmittanceTexture& transmittance_texture,
		const ReducedScatteringTexture& single_rayleigh_scattering_texture,
		const ReducedScatteringTexture& single_mie_scattering_texture,
		const ScatteringTexture& multiple_scattering_texture,
		const IrradianceTexture& irradiance_texture,
		const int order)
		: TernaryFunction(
			RadianceDensitySpectrum(-watt_per_cubic_meter_per_sr_per_nm)),
		atmosphere_parameters_(atmosphere_parameters),
		transmittance_texture_(transmittance_texture),
		single_rayleigh_scattering_texture_(single_rayleigh_scattering_texture),
		single_mie_scattering_texture_(single_mie_scattering_texture),
		multiple_scattering_texture_(multiple_scattering_texture),
		irradiance_texture_(irradiance_texture),
		order_(order) {}

	virtual const RadianceDensitySpectrum& Get(int i, int j, int k) const 
	{
		int index = i + SCATTERING_TEXTURE_WIDTH * (j + SCATTERING_TEXTURE_HEIGHT * k);
		if (value_[index][0] < 0.0 * watt_per_cubic_meter_per_sr_per_nm) {
			value_[index] = ComputeScatteringDensityTexture(
				atmosphere_parameters_, transmittance_texture_,
				single_rayleigh_scattering_texture_, single_mie_scattering_texture_,
				multiple_scattering_texture_, irradiance_texture_,
				float3(i + 0.5, j + 0.5, k + 0.5), order_);
		}
		return value_[index];
	}

private:
	const AtmosphereParameters& atmosphere_parameters_;
	const TransmittanceTexture& transmittance_texture_;
	const ReducedScatteringTexture& single_rayleigh_scattering_texture_;
	const ReducedScatteringTexture& single_mie_scattering_texture_;
	const ScatteringTexture& multiple_scattering_texture_;
	const IrradianceTexture& irradiance_texture_;
	const int order_;
};

//���ɢ��������㣬����2
class LazyMultipleScatteringTexture : public dimensional::TernaryFunction<
	SCATTERING_TEXTURE_WIDTH,
	SCATTERING_TEXTURE_HEIGHT,
	SCATTERING_TEXTURE_DEPTH,
	RadianceSpectrum> 
{
public:
	LazyMultipleScatteringTexture(
		const AtmosphereParameters& atmosphere_parameters,
		const TransmittanceTexture& transmittance_texture,
		const ScatteringDensityTexture& scattering_density_texture)
		: TernaryFunction(RadianceSpectrum(-watt_per_square_meter_per_sr_per_nm)),
		atmosphere_parameters_(atmosphere_parameters),
		transmittance_texture_(transmittance_texture),
		scattering_density_texture_(scattering_density_texture) {}

	virtual const RadianceSpectrum& Get(int i, int j, int k) const
	{
		int index = i + SCATTERING_TEXTURE_WIDTH * (j + SCATTERING_TEXTURE_HEIGHT * k);
		if (value_[index][0] < 0.0 * watt_per_square_meter_per_sr_per_nm) 
		{
			Number ignored;
			value_[index] = ComputeMultipleScatteringTexture(atmosphere_parameters_,
				transmittance_texture_, scattering_density_texture_,
				float3(i + 0.5, j + 0.5, k + 0.5), ignored);
		}
		return value_[index];
	}

private:
	const AtmosphereParameters& atmosphere_parameters_;
	const TransmittanceTexture& transmittance_texture_;
	const ScatteringDensityTexture& scattering_density_texture_;
};

//and, finally, a lazy ground irradiance texture:
class LazyIndirectIrradianceTexture : public dimensional::BinaryFunction<
	IRRADIANCE_TEXTURE_WIDTH,
	IRRADIANCE_TEXTURE_HEIGHT,
	IrradianceSpectrum> 
{
public:
	LazyIndirectIrradianceTexture(
		const AtmosphereParameters& atmosphere_parameters,
		const ReducedScatteringTexture& single_rayleigh_scattering_texture,
		const ReducedScatteringTexture& single_mie_scattering_texture,
		const ScatteringTexture& multiple_scattering_texture,
		int scattering_order)
		: BinaryFunction(IrradianceSpectrum(-watt_per_square_meter_per_nm)),
		atmosphere_parameters_(atmosphere_parameters),
		single_rayleigh_scattering_texture_(single_rayleigh_scattering_texture),
		single_mie_scattering_texture_(single_mie_scattering_texture),
		multiple_scattering_texture_(multiple_scattering_texture),
		scattering_order_(scattering_order) {}

	virtual const IrradianceSpectrum& Get(int i, int j) const 
	{
		int index = i + j * IRRADIANCE_TEXTURE_HEIGHT;
		if (value_[index][0] < 0.0 * watt_per_square_meter_per_nm) 
		{
			value_[index] = ComputeIndirectIrradianceTexture(atmosphere_parameters_,
				single_rayleigh_scattering_texture_,
				single_mie_scattering_texture_,
				multiple_scattering_texture_,
				float2(i + 0.5, j + 0.5),
				scattering_order_);
		}
		return value_[index];
	}

private:
	const AtmosphereParameters& atmosphere_parameters_;
	const ReducedScatteringTexture& single_rayleigh_scattering_texture_;
	const ReducedScatteringTexture& single_mie_scattering_texture_;
	const ScatteringTexture& multiple_scattering_texture_;
	int scattering_order_;
};

/*
�������ڿ����Լ����嵥Ԫ���ԡ� 
ÿ�����Զ���TestCase�����һ��ʵ��������һ��������������ʼ����atmosphere_parameters_�ֶΡ� 
��ע�⣬Ϊÿ����Ԫ���Դ����˴������ʵ����
*/

class FunctionsTest : public dimensional::TestCase
{
public:
	template<typename T>
	FunctionsTest(const std::string& name, T test) : TestCase("FunctionsTest " + name, static_cast<Test>(test))
	{
		atmosphere_parameters_.solar_irradiance[0] = kSolarIrradiance;
		atmosphere_parameters_.bottom_radius = kBottomRadius;
		atmosphere_parameters_.top_radius = kTopRadius;
		atmosphere_parameters_.rayleigh_density.layers[1] = DensityProfileLayer(
			0.0 * m, 1.0, -1.0 / kRayleighScaleHeight, 0.0 / m, 0.0);
		atmosphere_parameters_.rayleigh_scattering[0] = kRayleighScattering;
		atmosphere_parameters_.mie_density.layers[1] = DensityProfileLayer(
			0.0 * m, 1.0, -1.0 / kMieScaleHeight, 0.0 / m, 0.0);
		atmosphere_parameters_.mie_scattering[0] = kMieScattering;
		atmosphere_parameters_.mie_extinction[0] = kMieExtinction;
		atmosphere_parameters_.ground_albedo[0] = kGroundAlbedo;
		atmosphere_parameters_.mu_s_min = -1.0;
	}

	/*
	������������߽�ľ���:��⣬�Դ�ֱ����(mu = 1)������Rtop - r��ˮƽ����(mu = 0) ������sqrt(Rtop*Rtop - r*r)
	*/
	void TestDistanceToTopAtmosphereBoundary()
	{
		constexpr Length r = kBottomRadius * 0.2 + kTopRadius * 0.8;
		// Vertical ray, looking top.
		ExpectNear(
			kTopRadius - r,
			DistanceToTopAtmosphereBoundary(atmosphere_parameters_, r, 1.0),
			1.0 * m);
		// Horizontal ray.
		ExpectNear(
			sqrt(kTopRadius * kTopRadius - r * r),
			DistanceToTopAtmosphereBoundary(atmosphere_parameters_, r, 0.0),
			1.0 * m);
	}

	/*
	�͵�����ཻ:��鴹ֱ���߲��͵����ཻ���������¿���
	ͬ���ģ������΢���ڵ�ƽ�ߵ�����(mu = mu_horiz + e)���͵����ཻ
	������΢�ڵ�ƽ�����������(mu = mu_horiz - e)�͵����ཻ
	*/
	void TestRayIntersectsGround() 
	{
		constexpr Length r = kBottomRadius * 0.9 + kTopRadius * 0.1;
		Number mu_horizon = CosineOfHorizonZenithAngle(r);
		//mu = 1 ��ֱ��������
		ExpectFalse(RayIntersectsGround(atmosphere_parameters_, r, 1.0));
		//��΢���ڵ�ƽ�ߵ�����
		ExpectFalse(RayIntersectsGround(atmosphere_parameters_, r, mu_horizon + kEpsilon));
		//��΢���ڵ�ƽ�ߵ�����
		ExpectTrue(RayIntersectsGround(atmosphere_parameters_, r, mu_horizon - kEpsilon));
		//��ֱ���µ�����
		ExpectTrue(RayIntersectsGround(atmosphere_parameters_, r, -1.0));
	}

	/*
	������������߽���Ӿ�����:������ϴ�ֱ����,��ComputeOpticalLengthToTopAtmosphereBoundary����ֵ���ָ�������ֵ,���Խ�������:
	f(r->r_top)[exp( -((x - r_bottom) / K) ) dx] = K[exp(-((r - r_bottom) / K) - exp(-((r_top - r_bottom) / K)]  ;f��ʾ���ַ���
	K��scale height.
	ͬ���أ������ں㶨density profile�������������߽�Ĺ�ѧ�����ǵ����������߽�ľ��루ʹ��ˮƽ���ߣ���
	*/
	void TestComputeOpticalLengthToTopAtmosphereBoundary() 
	{
		constexpr Length r = kBottomRadius * 0.2 + kTopRadius * 0.8;
		constexpr Length h_r = r - kBottomRadius;
		constexpr Length h_top = kTopRadius - kBottomRadius;
		// Vertical ray, looking top.
		ExpectNear(
			kRayleighScaleHeight * (exp(-h_r / kRayleighScaleHeight) -
				exp(-h_top / kRayleighScaleHeight)),
			ComputeOpticalLengthToTopAtmosphereBoundary(atmosphere_parameters_,
				atmosphere_parameters_.rayleigh_density, r, 1.0),
			1.0 * m);
		// Horizontal ray, no exponential density fall off.
		SetUniformAtmosphere();
		ExpectNear(
			sqrt(kTopRadius * kTopRadius - r * r),
			ComputeOpticalLengthToTopAtmosphereBoundary(atmosphere_parameters_,
				atmosphere_parameters_.rayleigh_density, r, 0.0),
			1.0 * m);
	}

	/*
	�������ܶ� profiles:���desity profiles��ָ�������ԣ������ܶȣ�һ������㱻��ȷ����
	*/
	void TestGetProfileDensity() 
	{
		DensityProfile profile;
		// һ��ָ���ܶ�
		profile.layers[1] = DensityProfileLayer(0.0 * m, 1.0, -1.0 / km, 0.0 / km, 0.0);
		ExpectEquals(exp(-2.0), GetProfileDensity(profile, 2.0 * km)());
		// Only one layer, with (clamped) affine density.
		profile.layers[1] = DensityProfileLayer(0.0 * m, 0.0, 0.0 / km, -0.5 / km, 1.0);
		ExpectEquals(1.0, GetProfileDensity(profile, 0.0 * km)());
		ExpectEquals(0.5, GetProfileDensity(profile, 1.0 * km)());
		ExpectEquals(0.0, GetProfileDensity(profile, 3.0 * km)());

		// Two layers, with (clamped) affine density.
		profile.layers[0] = DensityProfileLayer(25.0 * km, 0.0, 0.0 / km, 1.0 / (15.0 * km), -2.0 / 3.0);
		profile.layers[1] = DensityProfileLayer(0.0 * km, 0.0, 0.0 / km, -1.0 / (15.0 * km), 8.0 / 3.0);
		ExpectEquals(0.0, GetProfileDensity(profile, 0.0 * km)());
		ExpectNear(0.0, GetProfileDensity(profile, 10.0 * km)(), kEpsilon);
		ExpectNear(1.0, GetProfileDensity(profile, 25.0 * km)(), kEpsilon);
		ExpectNear(0.0, GetProfileDensity(profile, 40.0 * km)(), kEpsilon);
		ExpectEquals(0.0, GetProfileDensity(profile, 50.0 * km)());
	}

	/*
	������������߽��͸���ʣ�
	��鴹ֱ���ߣ��鿴ComputeTransmittanceToTopAtmosphereBoundary�е���ֵ���ָ���Ԥ�ڽ��
	���Լ���exponential profile�Ľ����⣨ʹ������Ĺ�ѧ���ȷ��̣���triangular profile���������ڳ�����
	ͬ���أ�������ˮƽ���ߣ�û��Mieɢ�䣬���Ҿ��о����ܶȵĿ������ӣ���ѧ�����rayleighɢ��ϵ�����Ե�����������߽�ľ��롣
	*/
	void TestComputeTransmittanceToTopAtmosphereBoundary() 
	{
		constexpr Length r = kBottomRadius * 0.2 + kTopRadius * 0.8;
		constexpr Length h_r = r - kBottomRadius;
		constexpr Length h_top = kTopRadius - kBottomRadius;
		//��ֱ��������
		Number rayleigh_optical_depth = kRayleighScattering * kRayleighScaleHeight *
			(exp(-h_r / kRayleighScaleHeight) - exp(-h_top / kRayleighScaleHeight));
		Number mie_optical_depth = kMieExtinction * kMieScaleHeight *
			(exp(-h_r / kMieScaleHeight) - exp(-h_top / kMieScaleHeight));

		ExpectNear(
			exp(-(rayleigh_optical_depth + mie_optical_depth)),
			ComputeTransmittanceToTopAtmosphereBoundary(
				atmosphere_parameters_, r, 1.0)[0],
			Number(kEpsilon));
		// Vertical ray, looking up, no Rayleigh or Mie, only absorption, with a
		// triangular profile (whose integral is equal to 15km).
		//���ϴ�ֱ���ߣ�û��Rayleigh����Mie��ֻ��һ��triangular profile absorption
		atmosphere_parameters_.rayleigh_density.layers[1] = DensityProfileLayer();
		atmosphere_parameters_.mie_density.layers[1] = DensityProfileLayer();
		atmosphere_parameters_.absorption_density.layers[0] = DensityProfileLayer(25.0 * km, 0.0, 0.0 / km, 1.0 / (15.0 * km), -2.0 / 3.0);
		atmosphere_parameters_.absorption_density.layers[1] = DensityProfileLayer(0.0 * km, 0.0, 0.0 / km, -1.0 / (15.0 * km), 8.0 / 3.0);
		atmosphere_parameters_.absorption_extinction[0] = 0.02 / km;

		ExpectNear(
			exp(-Number(0.02 * 15.0)),
			ComputeTransmittanceToTopAtmosphereBoundary(
				atmosphere_parameters_, kBottomRadius, 1.0)[0],
			Number(kEpsilon));
		// Horizontal ray, uniform atmosphere without aerosols.
		//ˮƽ���ߣ����ȴ����㣬û����������(Mie)
		SetUniformAtmosphere();
		RemoveAerosols();
		ExpectNear(
			exp(-kRayleighScattering * sqrt(kTopRadius * kTopRadius - r * r)),
			ComputeTransmittanceToTopAtmosphereBoundary(
				atmosphere_parameters_, r, 0.0)[0],
			Number(kEpsilon));
	}

	/*
	�������꣺�����ڴ�СΪn����������0�����ģ�����������0.5 / n������ӳ�䵽0
	�������ص�����Ϊn-1�����������괦�� n-0.5��/ n��ӳ�䵽1����֮��Ȼ���� 
	��󣬼��ӳ�亯��������ʵ�����Ƿ��෴�������ǵ����(���)Ӧ�ø���Identiy��������
	*/
	void TestGetTextureCoordFromUnitRange() 
	{
		ExpectNear(0.5 / 10.0, GetTextureCoordFromUnitRange(0.0, 10)(), kEpsilon);
		ExpectNear(9.5 / 10.0, GetTextureCoordFromUnitRange(1.0, 10)(), kEpsilon);

		ExpectNear(0.0, GetUnitRangeFromTextureCoord(0.5 / 10.0, 10)(), kEpsilon);
		ExpectNear(1.0, GetUnitRangeFromTextureCoord(9.5 / 10.0, 10)(), kEpsilon);

		ExpectNear(1.0 / 3.0, GetUnitRangeFromTextureCoord(GetTextureCoordFromUnitRange(1.0 / 3.0, 10), 10)(), kEpsilon);
	}

	/*
	ӳ�䵽͸�����������꣺���r�ı߽�ֵ(r_bottom��r_top)��mu�ı߽�ֵ(mu_horiz��1)�Ƿ�ӳ�䵽͸��������ı߽�texels
	*/
	void TestGetTransmittanceTextureUvFromRMu() 
	{
		float2 uv = GetTransmittanceTextureUvFromRMu(atmosphere_parameters_, kTopRadius, 1.0);
		ExpectNear(0.5 / TRANSMITTANCE_TEXTURE_WIDTH, uv.x(), kEpsilon);
		ExpectNear(1.0 - 0.5 / TRANSMITTANCE_TEXTURE_HEIGHT, uv.y(), kEpsilon);

		Number top_mu_horizon = CosineOfHorizonZenithAngle(kTopRadius);
		uv = GetTransmittanceTextureUvFromRMu(
			atmosphere_parameters_, kTopRadius, top_mu_horizon);
		ExpectNear(1.0 - 0.5 / TRANSMITTANCE_TEXTURE_WIDTH, uv.x(), kEpsilon);
		ExpectNear(1.0 - 0.5 / TRANSMITTANCE_TEXTURE_HEIGHT, uv.y(), kEpsilon);

		uv = GetTransmittanceTextureUvFromRMu(
			atmosphere_parameters_, kBottomRadius, 1.0);
		ExpectNear(0.5 / TRANSMITTANCE_TEXTURE_WIDTH, uv.x(), kEpsilon);
		ExpectNear(0.5 / TRANSMITTANCE_TEXTURE_HEIGHT, uv.y(), kEpsilon);

		uv = GetTransmittanceTextureUvFromRMu(
			atmosphere_parameters_, kBottomRadius, 0.0);
		ExpectNear(1.0 - 0.5 / TRANSMITTANCE_TEXTURE_WIDTH, uv.x(), kEpsilon);
		ExpectNear(0.5 / TRANSMITTANCE_TEXTURE_HEIGHT, uv.y(), kEpsilon);
	}

	/*
	��͸�������������ӳ�䣺���͸��������ı߽����ص������Ƿ�ӳ�䵽r��r_bottom��r_top����mu��mu_horiz��1���ı߽�ֵ�� 
	��󣬼��ӳ�亯��������ʵ�����Ƿ��෴�������ǵĳ�Ӧ�ø���Identity��������
	*/
	void TestGetRMuFromTransmittanceTextureUv() 
	{
		Length r;
		Number mu;
		GetRMuFromTransmittanceTextureUv(
			atmosphere_parameters_,
			float2(0.5 / TRANSMITTANCE_TEXTURE_WIDTH,1.0 - 0.5 / TRANSMITTANCE_TEXTURE_HEIGHT),
			r, mu);
		ExpectNear(kTopRadius, r, 1.0 * m);
		ExpectNear(1.0, mu(), kEpsilon);

		GetRMuFromTransmittanceTextureUv(
			atmosphere_parameters_,
			float2(1.0 - 0.5 / TRANSMITTANCE_TEXTURE_WIDTH,1.0 - 0.5 / TRANSMITTANCE_TEXTURE_HEIGHT),
			r, mu);
		ExpectNear(kTopRadius, r, 1.0 * m);
		ExpectNear(CosineOfHorizonZenithAngle(kTopRadius)(),mu(),kEpsilon);

		GetRMuFromTransmittanceTextureUv(
			atmosphere_parameters_,
			float2(0.5 / TRANSMITTANCE_TEXTURE_WIDTH,0.5 / TRANSMITTANCE_TEXTURE_HEIGHT),
			r, mu);
		ExpectNear(kBottomRadius, r, 1.0 * m);
		ExpectNear(1.0, mu(), kEpsilon);

		GetRMuFromTransmittanceTextureUv(
			atmosphere_parameters_,
			float2(1.0 - 0.5 / TRANSMITTANCE_TEXTURE_WIDTH,0.5 / TRANSMITTANCE_TEXTURE_HEIGHT),
			r, mu);
		ExpectNear(kBottomRadius, r, 1.0 * m);
		ExpectNear(0.0, mu(), kEpsilon);

		GetRMuFromTransmittanceTextureUv(
			atmosphere_parameters_,
			GetTransmittanceTextureUvFromRMu(atmosphere_parameters_,kBottomRadius * 0.2 + kTopRadius * 0.8, 0.25),
			r, mu);
		ExpectNear(kBottomRadius * 0.2 + kTopRadius * 0.8, r, 1.0 * m);
		ExpectNear(0.25, mu(), kEpsilon);
	}

	/*
	͸����������������Ƿ����붥�������߽���ͬ��͸���ʣ���������e��
	����������ʹ��ComputeTransmittanceToTopAtmosphereBoundaryֱ�Ӽ��㣬����ͨ��Ԥ�ȼ����͸���������е�˫���Բ�ֵ���ҡ�
	*/
	void TestGetTransmittanceToTopAtmosphereBoundary() 
	{
		LazyTransmittanceTexture transmittance_texture(atmosphere_parameters_);

		const Length r = kBottomRadius * 0.2 + kTopRadius * 0.8;
		const Number mu = 0.4;
		ExpectNear(
			ComputeTransmittanceToTopAtmosphereBoundary(
				atmosphere_parameters_, r, mu)[0],
			GetTransmittanceToTopAtmosphereBoundary(
				atmosphere_parameters_, transmittance_texture, r, mu)[0],
			Number(kEpsilon));
	}

	/*
	͸�����������GetTransmittance������Ԥ�ȼ����͸�������������������˫���Բ�ֵ���ң�������Ԥ�ڽ��
	��ĳЩ����£����Խ�������������û�����ܽ��ҿ��������ܶȾ���ʱ�� ��ѧ��Ⱦ���Rayleighɢ��ϵ�����Դ����д����ľ��롣
	*/
	void TestComputeAndGetTransmittance() 
	{
		SetUniformAtmosphere();
		RemoveAerosols();
		LazyTransmittanceTexture transmittance_texture(atmosphere_parameters_);

		const Length r = kBottomRadius * 0.2 + kTopRadius * 0.8;
		const Length d = (kTopRadius - kBottomRadius) * 0.1;

		//ˮƽ���ߣ��ӵײ�������߽�
		ExpectNear(
			exp(-kRayleighScattering * d),
			GetTransmittance(atmosphere_parameters_, transmittance_texture,kBottomRadius, 0.0, d, false /* ray_intersects_ground */)[0],
			Number(kEpsilon));
	
		//���ϵģ�������ֱ������
		ExpectNear(
			exp(-kRayleighScattering * d),
			GetTransmittance(atmosphere_parameters_, transmittance_texture, r, 0.7, d,false /* ray_intersects_ground */)[0],
			Number(kEpsilon));

		//���µģ�������ֱ������
		ExpectNear(
			exp(-kRayleighScattering * d),
			GetTransmittance(atmosphere_parameters_, transmittance_texture, r, -0.7, d,RayIntersectsGround(atmosphere_parameters_, r, -0.7))[0],
			Number(kEpsilon));
	}

	/*
	����ɢ����֣����ComputeSingleScatteringIntegrand�еļ��㣨ʹ��Ԥ�ȼ����͸������������Ԥ�ڽ������3������£����Խ�������˽����

	1)
	��ֱ���ߣ��ӵ������Ͽ���̫�����춥��ɢ����r�����������������㣺
		�Ӵ����ײ���������͸���ʡ�
		���漰�Ӵ����ĵײ���������Rayleigh��Mie��ѧ��ȣ��������ʽ:
		Kext*��[r_top,r_bottom]exp(-(x-r_bottom)/K)dx = Kext*K [1-exp(-(r_top-r_bottom)/K)]
		��r������ֵ�ܶ�:exp(-(r-r_bottom)/K)

	2��
	��ֱ���ߣ��Ӷ���������߽縩�ӣ�̫��λ���춥��ɢ����r�����������������㣺
		�Ӵ���������r��͸���ʣ����ҷ��ء�
		���漰�Ӵ���������r����2����Rayleigh��Mie��ѧ��ȣ�����ʽΪ��
		Kext*K*[exp(-(r-r_bottom)/K)-exp(-(r_top-r_bottom)/K)]
		��r����ֵ�ܶ�:exp(-(r-r_bottom)/K)

	3)
	�ӵ����ˮƽ���ߣ�̫���ڵ�ƽ�ߣ�ɢ�����۲��ߵľ���d,û�����ܽ��;����ܶȵĿ������ӡ�
	Ȼ�󣬱�������ֻ�Ǵӵ��浽���������߽��ˮƽ���ߵ�͸���ʣ��Ѿ���TestComputeTransmittanceToTopAtmosphereBoundary�м��㣩��
	*/
	void TestComputeSingleScatteringIntegrand()
	{
		LazyTransmittanceTexture transmittance_texture(atmosphere_parameters_);

		// Vertical ray, from bottom to top atmosphere boundary, scattering at
		// middle of ray, scattering angle equal to 0.
		//�ӵײ�������������߽�Ĵ�ֱ���ߣ��������е�ɢ�䣬nu�Ƕ�Ϊ0
		const Length h_top = kTopRadius - kBottomRadius;
		const Length h = h_top / 2.0;
		DimensionlessSpectrum rayleigh;
		DimensionlessSpectrum mie;
		ComputeSingleScatteringIntegrand(atmosphere_parameters_, transmittance_texture,kBottomRadius, 1.0, 1.0, 1.0, h, false, rayleigh, mie);

		Number rayleigh_optical_depth = kRayleighScattering * kRayleighScaleHeight * (1.0 - exp(-h_top / kRayleighScaleHeight));
		Number mie_optical_depth = kMieExtinction * kMieScaleHeight * (1.0 - exp(-h_top / kMieScaleHeight));
		ExpectNear(
			exp(-rayleigh_optical_depth - mie_optical_depth) * exp(-h / kRayleighScaleHeight),
			rayleigh[0],
			Number(kEpsilon));

		ExpectNear(exp(-rayleigh_optical_depth - mie_optical_depth) * exp(-h / kMieScaleHeight),
			mie[0],
			Number(kEpsilon));

		// Vertical ray, top to middle of atmosphere, scattering angle 180 degrees.
		//�����㶥�����м�Ĵ�ֱ���ߣ�nuΪ180��-1
		ComputeSingleScatteringIntegrand(atmosphere_parameters_, transmittance_texture,kTopRadius, -1.0, 1.0, -1.0, h, true, rayleigh, mie);

		rayleigh_optical_depth = 2.0 * kRayleighScattering * kRayleighScaleHeight *
			(exp(-h / kRayleighScaleHeight) - exp(-h_top / kRayleighScaleHeight));

		mie_optical_depth = 2.0 * kMieExtinction * kMieScaleHeight *
			(exp(-h / kMieScaleHeight) - exp(-h_top / kMieScaleHeight));

		ExpectNear(exp(-rayleigh_optical_depth - mie_optical_depth) * exp(-h / kRayleighScaleHeight),
			rayleigh[0],
			Number(kEpsilon));
		ExpectNear(exp(-rayleigh_optical_depth - mie_optical_depth) * exp(-h / kMieScaleHeight),
			mie[0],
			Number(kEpsilon));

		// Horizontal ray, from bottom to top atmosphere boundary, scattering at
		// 50km, scattering angle equal to 0, uniform atmosphere, no aerosols.
		//�ӵײ��������㶥����ˮƽ����,��50km��ɢ�䣬nuΪ0���������ȣ������ܽ�
		transmittance_texture.Clear();
		SetUniformAtmosphere();
		RemoveAerosols();
		ComputeSingleScatteringIntegrand(atmosphere_parameters_, transmittance_texture,kBottomRadius, 0.0, 0.0, 1.0, 50.0 * km, false, rayleigh, mie);
		rayleigh_optical_depth = kRayleighScattering * sqrt(kTopRadius * kTopRadius - kBottomRadius * kBottomRadius);
		ExpectNear(
			exp(-rayleigh_optical_depth),
			rayleigh[0],
			Number(kEpsilon));
	}

	/*
	������Ĵ����߽�ľ��룺
	��Ӧ��ֱ���Ͽ�(mu = 1)�����߼��ֵΪr_op - r
	ˮƽ����(mu = 0)Ϊsqrt(r_top*r_top - r*r)
	��ֱ���¿�������(mu=-1) r-r_bottom
	*/
	void TestDistanceToNearestAtmosphereBoundary()
	{
		constexpr Length r = kBottomRadius * 0.2 + kTopRadius * 0.8;
		// ��ֱ��������
		ExpectNear(
			kTopRadius - r,
			DistanceToNearestAtmosphereBoundary(atmosphere_parameters_, r, 1.0,RayIntersectsGround(atmosphere_parameters_, r, 1.0)),
			1.0 * m);
		// ˮƽ����
		ExpectNear(
			sqrt(kTopRadius * kTopRadius - r * r),
			DistanceToNearestAtmosphereBoundary(atmosphere_parameters_, r, 0.0,RayIntersectsGround(atmosphere_parameters_, r, 0.0)),
			1.0 * m);
		// ��ֱ��������
		ExpectNear(
			r - kBottomRadius,
			DistanceToNearestAtmosphereBoundary(atmosphere_parameters_, r, -1.0,RayIntersectsGround(atmosphere_parameters_, r, -1.0)),
			1.0 * m);
	}

	/*
	����ɢ�䣺���ComputeSingleScattering�е���ֵ�����Ƿ����Ԥ�ڽ������2������£����Խ���������֣�
	1)
	��ֱ���ߣ��ӵ������Ͽ���̫�����춥��
	����ɢ����־�����ʽ������ʡ����̫�����նȺ�Mie�����Լ򻯱��ʽ;����μ�TestComputeSingleScatteringIntegrand����
	Ksca*��[r_top,r_bottom] exp(-Kext*K [1 - exp(-(r_top-r_bottom)/K)]) * exp(-(r-r_bottom)/K)dr
	����
	Ksca * exp(-Kext*K [1 - exp(-(r_top-r_bottom)/K)]) * K [1-exp(-(r_top-r_bottom)K)]

	2)
	��ֱ���ߣ��Ӷ��������߽縩�ӣ�̫�����춥��û�����ܽ���
	����ɢ����־�����ʽ������ʡ����̫�����ն�;����μ�TestComputeSingleScatteringIntegrand����
	Ksca*��[r_top,r_bottom] exp(-2*Ksca*K [exp(-(r-r_bottom)/K) - exp(-(r_top-r_bottom)/K)]) * exp(-(r-r_bottom)/K)dr
	���У������u = exp(-(r - r_bottom) / K),����
	0.5*(1 - exp(-2*Ksca*K(1 - exp(-(r_top-r_bottom) / K))))
	*/
	void TestComputeSingleScattering() 
	{
		LazyTransmittanceTexture transmittance_texture(atmosphere_parameters_);

		// Vertical ray, from bottom atmosphere boundary, scattering angle 0.
		//���´�ֱ���ϵ�������߽磬nu = 0
		const Length h_top = kTopRadius - kBottomRadius;
		IrradianceSpectrum rayleigh;
		IrradianceSpectrum mie;
		ComputeSingleScattering(atmosphere_parameters_, transmittance_texture,kBottomRadius, 1.0, 1.0, 1.0, false, rayleigh, mie);
		Number rayleigh_optical_depth = kRayleighScattering * kRayleighScaleHeight * (1.0 - exp(-h_top / kRayleighScaleHeight));
		Number mie_optical_depth = kMieExtinction * kMieScaleHeight * (1.0 - exp(-h_top / kMieScaleHeight));
		// The relative error is about 1% here.
		ExpectNear(
			Number(1.0),
			rayleigh[0] / (kSolarIrradiance * rayleigh_optical_depth * exp(-rayleigh_optical_depth - mie_optical_depth)),
			Number(10.0 * kEpsilon));
		ExpectNear(
			Number(1.0),
			mie[0] / (kSolarIrradiance * mie_optical_depth * kMieScattering / kMieExtinction * exp(-rayleigh_optical_depth - mie_optical_depth)),
			Number(10.0 * kEpsilon));

		// Vertical ray, from top atmosphere boundary, scattering angle 180 degrees,
		// no aerosols.
		transmittance_texture.Clear();
		RemoveAerosols();
		ComputeSingleScattering(atmosphere_parameters_, transmittance_texture,kTopRadius, -1.0, 1.0, -1.0, true, rayleigh, mie);
		ExpectNear(
			Number(1.0),
			rayleigh[0] / (kSolarIrradiance * 0.5 * (1.0 - exp(-2.0 * kRayleighScaleHeight * kRayleighScattering * (1.0 - exp(-h_top / kRayleighScaleHeight))))),
			Number(2.0 * kEpsilon));
		ExpectNear(0.0, mie[0].to(watt_per_square_meter_per_nm), kEpsilon);
	}

	/*
	Rayleigh��Mie��λ�����������Щ��λ����������������ϵĻ����Ƿ�Ϊ1��
	*/
	void TestPhaseFunctions() 
	{
		Number rayleigh_integral = 0.0;
		Number mie_integral = 0.0;
		const unsigned int N = 100;
		for (unsigned int i = 0; i < N; ++i) 
		{
			Angle theta = (i + 0.5) * pi / N;
			SolidAngle domega = sin(theta) * (PI / N) * (2.0 * PI) * sr;
			rayleigh_integral = rayleigh_integral + RayleighPhaseFunction(cos(theta)) * domega;
			mie_integral = mie_integral + MiePhaseFunction(0.8, cos(theta)) * domega;
		}
		ExpectNear(1.0, rayleigh_integral(), 2.0 * kEpsilon);
		ExpectNear(1.0, mie_integral(), 2.0 * kEpsilon);
	}

	/*
	ӳ�䵽ɢ���������꣺
	���r(r_top,r_bottom),mu(-1,mu_horiz,1),mu_s(-1,1)��nu(-1,1)�ı߽�ֵ�Ƿ�ӳ�䵽���� ɢ������ı߽����ء�
	*/
	void TestGetScatteringTextureUvwzFromRMuMuSNu() 
	{
		ExpectNear(
			0.5 / SCATTERING_TEXTURE_R_SIZE,
			GetScatteringTextureUvwzFromRMuMuSNu(atmosphere_parameters_, kBottomRadius, 0.0, 0.0, 0.0, false).w(),
			kEpsilon);

		ExpectNear(
			1.0 - 0.5 / SCATTERING_TEXTURE_R_SIZE,
			GetScatteringTextureUvwzFromRMuMuSNu(atmosphere_parameters_, kTopRadius, 0.0, 0.0, 0.0, false).w(),
			kEpsilon);

		const Length r = (kTopRadius + kBottomRadius) / 2.0;
		const Number mu = CosineOfHorizonZenithAngle(r);
		ExpectNear(
			0.5 / SCATTERING_TEXTURE_MU_SIZE,
			GetScatteringTextureUvwzFromRMuMuSNu(atmosphere_parameters_, r, mu, 0.0, 0.0, true).z(),
			kEpsilon);
		ExpectNear(
			1.0 - 0.5 / SCATTERING_TEXTURE_MU_SIZE,
			GetScatteringTextureUvwzFromRMuMuSNu(atmosphere_parameters_, r, mu, 0.0, 0.0, false).z(),
			kEpsilon);
		ExpectTrue(GetScatteringTextureUvwzFromRMuMuSNu(atmosphere_parameters_, r, -1.0, 0.0, 0.0, true).z() < 0.5);
		ExpectTrue(GetScatteringTextureUvwzFromRMuMuSNu(atmosphere_parameters_, r, 1.0, 0.0, 0.0, false).z() > 0.5);

		ExpectNear(
			0.5 / SCATTERING_TEXTURE_MU_S_SIZE,
			GetScatteringTextureUvwzFromRMuMuSNu(atmosphere_parameters_, kBottomRadius, 0.0, -1.0, 0.0, false).y(),
			kEpsilon);
		ExpectNear(
			1.0 - 0.5 / SCATTERING_TEXTURE_MU_S_SIZE,
			GetScatteringTextureUvwzFromRMuMuSNu(atmosphere_parameters_, kBottomRadius, 0.0, 1.0, 0.0, false).y(),
			kEpsilon);

		ExpectNear(
			0.5 / SCATTERING_TEXTURE_MU_S_SIZE,
			GetScatteringTextureUvwzFromRMuMuSNu(atmosphere_parameters_, kTopRadius, 0.0, -1.0, 0.0, false).y(),
			kEpsilon);
		ExpectNear(
			1.0 - 0.5 / SCATTERING_TEXTURE_MU_S_SIZE,
			GetScatteringTextureUvwzFromRMuMuSNu(atmosphere_parameters_, kTopRadius, 0.0, 1.0, 0.0, false).y(),
			kEpsilon);

		ExpectNear(
			0.0,
			GetScatteringTextureUvwzFromRMuMuSNu(atmosphere_parameters_, kBottomRadius, 0.0, 0.0, -1.0, false).x(),
			kEpsilon);
		ExpectNear(
			1.0,
			GetScatteringTextureUvwzFromRMuMuSNu(atmosphere_parameters_, kBottomRadius, 0.0, 0.0, 1.0, false).x(),
			kEpsilon);
	}

	/*
	��ɢ����������ӳ�䣺
	���ɢ������ı߽����ص������Ƿ�ӳ�䵽r(r_top,t_bottom),mu(-1,mu_horiz,1),mu_s(-1,1),nu(-1,1)
	��󣬼��ӳ�亯��������ʵ�����Ƿ��෴�������ǵ����Ӧ�ø�����ʶ��������
	*/
	void TestGetRMuMuSNuFromScatteringTextureUvwz() 
	{
		Length r;
		Number mu;
		Number mu_s;
		Number nu;
		bool ray_r_mu_intersects_ground;
		GetRMuMuSNuFromScatteringTextureUvwz(atmosphere_parameters_,
			float4(0.0,
				0.5 / SCATTERING_TEXTURE_MU_S_SIZE,
				0.5 / SCATTERING_TEXTURE_MU_SIZE,
				0.5 / SCATTERING_TEXTURE_R_SIZE),
			r, mu, mu_s, nu, ray_r_mu_intersects_ground);
		ExpectNear(kBottomRadius, r, 1.0 * m);
		GetRMuMuSNuFromScatteringTextureUvwz(atmosphere_parameters_,
			float4(0.0,
				0.5 / SCATTERING_TEXTURE_MU_S_SIZE,
				0.5 / SCATTERING_TEXTURE_MU_SIZE,
				1.0 - 0.5 / SCATTERING_TEXTURE_R_SIZE),
			r, mu, mu_s, nu, ray_r_mu_intersects_ground);
		ExpectNear(kTopRadius, r, 1.0 * m);

		GetRMuMuSNuFromScatteringTextureUvwz(atmosphere_parameters_,
			float4(0.0,
				0.5 / SCATTERING_TEXTURE_MU_S_SIZE,
				0.5 / SCATTERING_TEXTURE_MU_SIZE + kEpsilon,
				0.5),
			r, mu, mu_s, nu, ray_r_mu_intersects_ground);
		const Number mu_horizon = CosineOfHorizonZenithAngle(r);
		ExpectNear(mu_horizon, mu, Number(kEpsilon));
		ExpectTrue(mu <= mu_horizon);
		ExpectTrue(ray_r_mu_intersects_ground);
		GetRMuMuSNuFromScatteringTextureUvwz(atmosphere_parameters_,
			float4(0.0,
				0.5 / SCATTERING_TEXTURE_MU_S_SIZE,
				1.0 - 0.5 / SCATTERING_TEXTURE_MU_SIZE - kEpsilon,
				0.5),
			r, mu, mu_s, nu, ray_r_mu_intersects_ground);
		ExpectNear(mu_horizon, mu, Number(5.0 * kEpsilon));
		ExpectTrue(mu >= mu_horizon);
		ExpectFalse(ray_r_mu_intersects_ground);

		GetRMuMuSNuFromScatteringTextureUvwz(atmosphere_parameters_,
			float4(0.0,
				0.5 / SCATTERING_TEXTURE_MU_S_SIZE,
				0.5 / SCATTERING_TEXTURE_MU_SIZE,
				0.5 / SCATTERING_TEXTURE_R_SIZE),
			r, mu, mu_s, nu, ray_r_mu_intersects_ground);
		ExpectNear(-1.0, mu_s(), kEpsilon);
		GetRMuMuSNuFromScatteringTextureUvwz(atmosphere_parameters_,
			float4(0.0,
				1.0 - 0.5 / SCATTERING_TEXTURE_MU_S_SIZE,
				0.5 / SCATTERING_TEXTURE_MU_SIZE,
				0.5 / SCATTERING_TEXTURE_R_SIZE),
			r, mu, mu_s, nu, ray_r_mu_intersects_ground);
		ExpectNear(1.0, mu_s(), kEpsilon);

		GetRMuMuSNuFromScatteringTextureUvwz(atmosphere_parameters_,
			float4(0.0,
				0.5 / SCATTERING_TEXTURE_MU_S_SIZE,
				0.5 / SCATTERING_TEXTURE_MU_SIZE,
				0.5 / SCATTERING_TEXTURE_R_SIZE),
			r, mu, mu_s, nu, ray_r_mu_intersects_ground);
		ExpectNear(-1.0, nu(), kEpsilon);
		GetRMuMuSNuFromScatteringTextureUvwz(atmosphere_parameters_,
			float4(1.0,
				0.5 / SCATTERING_TEXTURE_MU_S_SIZE,
				0.5 / SCATTERING_TEXTURE_MU_SIZE,
				0.5 / SCATTERING_TEXTURE_R_SIZE),
			r, mu, mu_s, nu, ray_r_mu_intersects_ground);
		ExpectNear(1.0, nu(), kEpsilon);

		GetRMuMuSNuFromScatteringTextureUvwz(atmosphere_parameters_,
			GetScatteringTextureUvwzFromRMuMuSNu(atmosphere_parameters_,
				kBottomRadius, -1.0, 1.0, -1.0, true),
			r, mu, mu_s, nu, ray_r_mu_intersects_ground);
		ExpectNear(kBottomRadius, r, 1.0 * m);
		ExpectNear(-1.0, mu(), kEpsilon);
		ExpectNear(1.0, mu_s(), kEpsilon);
		ExpectNear(-1.0, nu(), kEpsilon);
		ExpectTrue(ray_r_mu_intersects_ground);

		GetRMuMuSNuFromScatteringTextureUvwz(atmosphere_parameters_,
			GetScatteringTextureUvwzFromRMuMuSNu(atmosphere_parameters_,
				kTopRadius, -1.0, 1.0, -1.0, true),
			r, mu, mu_s, nu, ray_r_mu_intersects_ground);
		ExpectNear(kTopRadius, r, 1.0 * m);
		ExpectNear(-1.0, mu(), kEpsilon);
		ExpectNear(1.0, mu_s(), kEpsilon);
		ExpectNear(-1.0, nu(), kEpsilon);
		ExpectTrue(ray_r_mu_intersects_ground);

		GetRMuMuSNuFromScatteringTextureUvwz(atmosphere_parameters_,
			GetScatteringTextureUvwzFromRMuMuSNu(atmosphere_parameters_,
			(kBottomRadius + kTopRadius) / 2.0, 0.2, 0.3, 0.4, false),
			r, mu, mu_s, nu, ray_r_mu_intersects_ground);
		ExpectNear((kBottomRadius + kTopRadius) / 2.0, r, 1.0 * m);
		ExpectNear(0.2, mu(), kEpsilon);
		ExpectNear(0.3, mu_s(), kEpsilon);
		ExpectNear(0.4, nu(), kEpsilon);
		ExpectFalse(ray_r_mu_intersects_ground);
	}

	/*
	����ɢ������
	��������Ƿ�����ͬ�ĵ���ɢ��ֵ����������e��
	����������ʹ��ComputeSingleScatteringֱ�Ӽ�����������ͨ��Ԥ�ȼ���ĵ���ɢ�������е�quadrilinearly��ֵ���ҡ�
	*/
	void TestComputeAndGetSingleScattering() 
	{
		LazyTransmittanceTexture transmittance_texture(atmosphere_parameters_);
		LazySingleScatteringTexture single_rayleigh_scattering_texture(
			atmosphere_parameters_, transmittance_texture, true);
		LazySingleScatteringTexture single_mie_scattering_texture(
			atmosphere_parameters_, transmittance_texture, false);

		//Vertical ray, from bottom atmosphere boundary, scattering angle 0.
			IrradianceSpectrum rayleigh = GetScattering(
				atmosphere_parameters_, single_rayleigh_scattering_texture,
				kBottomRadius, 1.0, 1.0, 1.0, false);
		IrradianceSpectrum mie = GetScattering(
			atmosphere_parameters_, single_mie_scattering_texture,
			kBottomRadius, 1.0, 1.0, 1.0, false);
		IrradianceSpectrum expected_rayleigh;
		IrradianceSpectrum expected_mie;
		ComputeSingleScattering(
			atmosphere_parameters_, transmittance_texture,
			kBottomRadius, 1.0, 1.0, 1.0, false, expected_rayleigh, expected_mie);
		ExpectNear(1.0, (rayleigh / expected_rayleigh)[0](), kEpsilon);
		ExpectNear(1.0, (mie / expected_mie)[0](), kEpsilon);

		// Vertical ray, from top atmosphere boundary, scattering angle 180 degrees.
		rayleigh = GetScattering(
			atmosphere_parameters_, single_rayleigh_scattering_texture,
			kTopRadius, -1.0, 1.0, -1.0, true);
		mie = GetScattering(
			atmosphere_parameters_, single_mie_scattering_texture,
			kTopRadius, -1.0, 1.0, -1.0, true);
		ComputeSingleScattering(
			atmosphere_parameters_, transmittance_texture,
			kTopRadius, -1.0, 1.0, -1.0, true, expected_rayleigh, expected_mie);
		ExpectNear(1.0, (rayleigh / expected_rayleigh)[0](), kEpsilon);
		ExpectNear(1.0, (mie / expected_mie)[0](), kEpsilon);

		// Horizontal ray, from bottom of atmosphere, scattering angle 90 degrees.
		rayleigh = GetScattering(
			atmosphere_parameters_, single_rayleigh_scattering_texture,
			kBottomRadius, 0.0, 0.0, 0.0, false);
		mie = GetScattering(
			atmosphere_parameters_, single_mie_scattering_texture,
			kBottomRadius, 0.0, 0.0, 0.0, false);
		ComputeSingleScattering(
			atmosphere_parameters_, transmittance_texture,
			kBottomRadius, 0.0, 0.0, 0.0, false, expected_rayleigh, expected_mie);
		// The relative error is quite large in this case, i.e. between 6 to 8%.
		ExpectNear(1.0, (rayleigh / expected_rayleigh)[0](), 1e-1);
		ExpectNear(1.0, (mie / expected_mie)[0](), 1e-1);

		// Ray just above the horizon, sun at the zenith.
		Number mu = CosineOfHorizonZenithAngle(kTopRadius);
		rayleigh = GetScattering(
			atmosphere_parameters_, single_rayleigh_scattering_texture,
			kTopRadius, mu, 1.0, mu, false);
		mie = GetScattering(
			atmosphere_parameters_, single_mie_scattering_texture,
			kTopRadius, mu, 1.0, mu, false);
		ComputeSingleScattering(
			atmosphere_parameters_, transmittance_texture,
			kTopRadius, mu, 1.0, mu, false, expected_rayleigh, expected_mie);
		ExpectNear(1.0, (rayleigh / expected_rayleigh)[0](), kEpsilon);
		ExpectNear(1.0, (mie / expected_mie)[0](), kEpsilon);
	}

	/*
	���ɢ�䣬����1��
	���ComputeScatteringDensity�е���ֵ�����Ƿ�����������¸���Ԥ�ڽ�������п��Խ�������Ļ��֣�
	1��
	�������(n - 1)�׵��������(radiance)�����з���������ͬ�ģ�����������淴������0��
	�����������ɢ����������з���������ͬ�ģ����ҵ���ɢ��ϵ�����������������(radiance)
	����λ������Ӱ������ˣ���Ϊ���������з����ϻ��֣�������λ���������з����ϵĻ���Ϊ1����
	2��
	�������(n - 1)�׵��������(radiance)Ϊ�㣬�������ն�(irradiance)���κεط�������ͬ�ģ�����͸����Ϊ1��
	Ȼ���ڵ�ƽ���ϣ���ˮƽ������ɢ��ķ���(radiance)��ɢ��ϵ�����Ե�����ն�(irradiance)�����Ե��淴���ʣ����ԦУ�LambertianBRDF����
	����2����Ϊ�������(radiance)�����ǵײ����򣬲���������λ����������Ժ͹۲����� - ������ɢ�����(radiance)��ˮƽ�����ѡ��
	*/
	void TestComputeScatteringDensity() 
	{
		RadianceSpectrum kRadiance(13.0 * watt_per_square_meter_per_sr_per_nm);
		TransmittanceTexture full_transmittance(DimensionlessSpectrum(1.0));
		ReducedScatteringTexture no_single_scattering(
			IrradianceSpectrum(0.0 * watt_per_square_meter_per_nm));
		ScatteringTexture uniform_multiple_scattering(kRadiance);
		IrradianceTexture no_irradiance(
			IrradianceSpectrum(0.0 * watt_per_square_meter_per_nm));

		RadianceDensitySpectrum scattering_density = ComputeScatteringDensity(
			atmosphere_parameters_, full_transmittance, no_single_scattering,
			no_single_scattering, uniform_multiple_scattering, no_irradiance,
			kBottomRadius, 0.0, 0.0, 1.0, 3);

		SpectralRadianceDensity kExpectedScatteringDensity =
			(kRayleighScattering + kMieScattering) * kRadiance[0];

		ExpectNear(1.0,(scattering_density[0] / kExpectedScatteringDensity)(),2.0 * kEpsilon);

		IrradianceSpectrum kIrradiance(13.0 * watt_per_square_meter_per_nm);
		IrradianceTexture uniform_irradiance(kIrradiance);

		ScatteringTexture no_multiple_scattering(
			RadianceSpectrum(0.0 * watt_per_square_meter_per_sr_per_nm));

		scattering_density = ComputeScatteringDensity(
			atmosphere_parameters_, full_transmittance, no_single_scattering,
			no_single_scattering, no_multiple_scattering, uniform_irradiance,
			kBottomRadius, 0.0, 0.0, 1.0, 3);
		kExpectedScatteringDensity = (kRayleighScattering + kMieScattering) *
			kGroundAlbedo / (2.0 * PI * sr) * kIrradiance[0];

		ExpectNear(1.0,(scattering_density[0] / kExpectedScatteringDensity)(),2.0 * kEpsilon);
	}

	/*
	���ɢ�䣬����2��
	���ComputeMultipleScattering�е���ֵ�����Ƿ������ĳЩ����¸���Ԥ�ڽ�������п��Խ���������ֵ������ 
	����Ӳ���1����ķ����(radiance)�����з����϶�����ͬ�ģ��������͸������1��
	��ôComputeMultipleScattering�е���ֵ����Ӧ�ü򵥵ص��ڷ����ʳ��Ծ�����������߽�ľ��롣 
	����������鿴�·��Ĵ�ֱ���ߣ��Լ��۲��ƽ�ߵĹ��ߡ�
	*/
	void TestComputeMultipleScattering()
	{
		RadianceDensitySpectrum kRadianceDensity(
			0.17 * watt_per_cubic_meter_per_sr_per_nm);
		TransmittanceTexture full_transmittance(DimensionlessSpectrum(1.0));
		ScatteringDensityTexture uniform_scattering_density(kRadianceDensity);

		// Vertical ray, looking bottom.
		Length r = kBottomRadius * 0.2 + kTopRadius * 0.8;
		Length distance_to_ground = r - kBottomRadius;
		ExpectNear(
			kRadianceDensity[0] * distance_to_ground,
			ComputeMultipleScattering(atmosphere_parameters_, full_transmittance,
				uniform_scattering_density, r, -1.0, 1.0, -1.0, true)[0],
			kRadianceDensity[0] * distance_to_ground * kEpsilon);

		// Ray just below the horizon.
		Number mu = CosineOfHorizonZenithAngle(kTopRadius);
		Length distance_to_horizon = sqrt(kTopRadius * kTopRadius - kBottomRadius * kBottomRadius);
		ExpectNear(
			kRadianceDensity[0] * distance_to_horizon,
			ComputeMultipleScattering(atmosphere_parameters_, full_transmittance,uniform_scattering_density, kTopRadius, mu, 1.0, mu, true)[0],
			kRadianceDensity[0] * distance_to_horizon * kEpsilon);
	}
private:

	/*
	����ǵ�Ԫ���ԣ� ����ֻ��Ҫʵ������ʹ�õ����ַ��������þ����ܶȵĿ������Ӻ����ܽ�����ȥ�����ܽ���
	*/
	void SetUniformAtmosphere() 
	{
		atmosphere_parameters_.rayleigh_density.layers[0] = DensityProfileLayer();
		atmosphere_parameters_.rayleigh_density.layers[1] = DensityProfileLayer(0.0 * m, 0.0, 0.0 / m, 0.0 / m, 1.0);
		atmosphere_parameters_.mie_density.layers[0] = DensityProfileLayer();
		atmosphere_parameters_.mie_density.layers[1] = DensityProfileLayer(0.0 * m, 0.0, 0.0 / m, 0.0 / m, 1.0);
		atmosphere_parameters_.absorption_density.layers[0] = DensityProfileLayer();
		atmosphere_parameters_.absorption_density.layers[1] = DensityProfileLayer();
	}

	void RemoveAerosols() 
	{
		atmosphere_parameters_.mie_scattering[0] = 0.0 / km;
		atmosphere_parameters_.mie_extinction[0] = 0.0 / km;
	}


	AtmosphereParameters atmosphere_parameters_;
};

//FunctionsTest distance_to_top_atmosphere_boundary(
//	"DistanceToTopAtmosphereBoundary",	&FunctionsTest::TestDistanceToTopAtmosphereBoundary);

//FunctionsTest ray_intersects_ground(
//	"RayIntersectsGround",&FunctionsTest::TestRayIntersectsGround);

//FunctionsTest get_profile_density(
//	"GetProfileDensity",&FunctionsTest::TestGetProfileDensity);

//FunctionsTest compute_transmittance_to_top_atmosphere_boundary(
//	"ComputeTransmittanceToTopAtmosphereBoundary",
//	&FunctionsTest::TestComputeTransmittanceToTopAtmosphereBoundary);

//FunctionsTest get_texture_coord_from_unit_range(
//	"GetTextureCoordFromUnitRange",
//	&FunctionsTest::TestGetTextureCoordFromUnitRange);

//FunctionsTest get_transmittance_texture_uv_from_rmu(
//	"GetTransmittanceTextureUvFromRMu",
//	&FunctionsTest::TestGetTransmittanceTextureUvFromRMu);

//FunctionsTest get_rmu_from_transmittance_texture_uv(
//	"GetRMuFromTransmittanceTextureUv",
//	&FunctionsTest::TestGetRMuFromTransmittanceTextureUv);

//FunctionsTest get_transmittance_to_top_atmosphere_boundary(
//	"GetTransmittanceToTopAtmosphereBoundary",
//	&FunctionsTest::TestGetTransmittanceToTopAtmosphereBoundary);

//FunctionsTest compute_and_get_transmittance(
//	"ComputeAndGetTransmittance",
//	&FunctionsTest::TestComputeAndGetTransmittance);

//FunctionsTest compute_single_scattering_integrand(
//	"ComputeSingleScatteringIntegrand",
//	&FunctionsTest::TestComputeSingleScatteringIntegrand);

//FunctionsTest distance_to_nearest_atmosphere_boundary(
//	"DistanceToNearestAtmosphereBoundary",
//	&FunctionsTest::TestDistanceToNearestAtmosphereBoundary);

//FunctionsTest compute_single_scattering(
//	"ComputeSingleScattering",
//	&FunctionsTest::TestComputeSingleScattering);

//FunctionsTest phase_functions(
//	"PhaseFunctions",
//	&FunctionsTest::TestPhaseFunctions);

//FunctionsTest get_scattering_texture_uvwz_from_rmumusnu(
//	"GetScatteringTextureUvwzFromRMuMuSNu",
//	&FunctionsTest::TestGetScatteringTextureUvwzFromRMuMuSNu);

//FunctionsTest get_rmumusnu_from_scattering_texture_uvwz(
//	"GetRMuMuSNuFromScatteringTextureUvwz",
//	&FunctionsTest::TestGetRMuMuSNuFromScatteringTextureUvwz);

//FunctionsTest compute_and_get_scattering(
//	"ComputeAndGetSingleScattering",
//	&FunctionsTest::TestComputeAndGetSingleScattering);

//���ɢ��
//FunctionsTest compute_scattering_density(
//	"ComputeScatteringDensity",
//	&FunctionsTest::TestComputeScatteringDensity);


NAME_SPACE_END
NAME_SPACE_END
NAME_SPACE_END





