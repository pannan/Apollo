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
ĳЩ��Ԫ������ҪԤ�ȼ����������Ϊ���룬������Ч��ԭ�����ǲ�ϣ��Ϊ��Ԥ�ȼ�������������
���ǵĽ���������ṩ�ӳټ�����������������ǵ�һ�γ��Զ�ȡ����ʱ�������ص�������
������Ҫ�ĵ�һ���������ӳ�͸����������ֵ��ζ�š���δ���㡱����
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

//�ӳٶ��ɢ������������1
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

//���ɢ���������㣬����2
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
	�������꣺�����ڴ�СΪn������������0�����ģ�����������0.5 / n������ӳ�䵽0
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
	ӳ�䵽͸�����������꣺���r�ı߽�ֵ(r_bottom��r_top)��mu�ı߽�ֵ(mu_horiz��1)�Ƿ�ӳ�䵽͸���������ı߽�texels
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

FunctionsTest get_transmittance_texture_uv_from_rmu(
	"GetTransmittanceTextureUvFromRMu",
	&FunctionsTest::TestGetTransmittanceTextureUvFromRMu);

NAME_SPACE_END
NAME_SPACE_END
NAME_SPACE_END




