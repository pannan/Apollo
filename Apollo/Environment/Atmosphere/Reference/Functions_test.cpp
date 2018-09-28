/*
该文件为实现我们的大气模型的GLSL函数提供单元测试。 我们从大气参数的一些（任意）值的定义开始：
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
//太阳辐照度
constexpr SpectralIrradiance kSolarIrradiance = 123.0 * watt_per_square_meter_per_nm;
//地球半径
constexpr Length kBottomRadius = 1000.0 * km;
//大气层顶部半径
constexpr Length kTopRadius = 1500.0 * km;
constexpr Length kScaleHeight = 60.0 * km;
constexpr Length kRayleighScaleHeight = 60.0 * km;
constexpr Length kMieScaleHeight = 30.0 * km;
constexpr ScatteringCoefficient kRayleighScattering = 0.001 / km;
constexpr ScatteringCoefficient kMieScattering = 0.0015 / km;
constexpr ScatteringCoefficient kMieExtinction = 0.002 / km;
constexpr Number kGroundAlbedo = 0.1;

/*
该辅助函数计算在某个半径r处天顶和地平线之间的角度的余弦值。 我们将用它来测试地平线上方或下方的光线。
*/
Number CosineOfHorizonZenithAngle(Length r)
{
	assert(r >= kBottomRadius);
	return -sqrt(1.0 - (kBottomRadius / r) * (kBottomRadius / r));
}

/*
某些单元测试需要预先计算的纹理作为输入，但出于效率原因，我们不希望为此预先计算整个纹理。
我们的解决方案是提供延迟计算的纹理，即在我们第一次尝试读取它们时计算纹素的纹理。
我们需要的第一种纹理是延迟透射纹理（负值意味着“尚未计算”）：
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

//我们也需要一个延迟单次散射纹理
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

//延迟多次散射纹理，步骤1
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

//多次散射纹理计算，步骤2
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
我们现在可以自己定义单元测试。 
每个测试都是TestCase子类的一个实例，它有一个从上述常量初始化的atmosphere_parameters_字段。 
请注意，为每个单元测试创建了此类的新实例。
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
	到顶部大气层边界的距离:检测，对垂直射线(mu = 1)距离是Rtop - r，水平射线(mu = 0) 距离是sqrt(Rtop*Rtop - r*r)
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
	和地面的相交:检查垂直射线不和地面相交，除非向下看。
	同样的，检测略微高于地平线的射线(mu = mu_horiz + e)不和地面相交
	但是略微在地平线下面的射线(mu = mu_horiz - e)和地面相交
	*/
	void TestRayIntersectsGround() 
	{
		constexpr Length r = kBottomRadius * 0.9 + kTopRadius * 0.1;
		Number mu_horizon = CosineOfHorizonZenithAngle(r);
		//mu = 1 垂直向上射线
		ExpectFalse(RayIntersectsGround(atmosphere_parameters_, r, 1.0));
		//略微高于地平线的射线
		ExpectFalse(RayIntersectsGround(atmosphere_parameters_, r, mu_horizon + kEpsilon));
		//略微低于地平线的射线
		ExpectTrue(RayIntersectsGround(atmosphere_parameters_, r, mu_horizon - kEpsilon));
		//垂直向下的射线
		ExpectTrue(RayIntersectsGround(atmosphere_parameters_, r, -1.0));
	}

	/*
	到顶部大气层边界的视觉长度:检测向上垂直射线,在ComputeOpticalLengthToTopAtmosphereBoundary的数值积分给出期望值,可以解析计算:
	f(r->r_top)[exp( -((x - r_bottom) / K) ) dx] = K[exp(-((r - r_bottom) / K) - exp(-((r_top - r_bottom) / K)]  ;f表示积分符号
	K是scale height.
	同样地，检查对于恒定density profile，到顶部大气边界的光学长度是到顶部大气边界的距离（使用水平射线）。
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
	大气层密度 profiles:检测desity profiles的指数，线性，常量密度，一层或两层被正确计算
	*/
	void TestGetProfileDensity() 
	{
		DensityProfile profile;
		// 一层指数密度
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
	到顶部大气层边界的透视率：
	检查垂直射线，查看ComputeTransmittanceToTopAtmosphereBoundary中的数值积分给出预期结果
	可以计算exponential profile的解析解（使用上面的光学长度方程）或triangular profile（例如用于臭氧）
	同样地，检查对于水平射线，没有Mie散射，并且具有均匀密度的空气分子，光学深度是rayleigh散射系数乘以到顶部大气层边界的距离。
	*/
	void TestComputeTransmittanceToTopAtmosphereBoundary() 
	{
		constexpr Length r = kBottomRadius * 0.2 + kTopRadius * 0.8;
		constexpr Length h_r = r - kBottomRadius;
		constexpr Length h_top = kTopRadius - kBottomRadius;
		//垂直向上射线
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
		//向上垂直射线，没有Rayleigh或者Mie，只有一个triangular profile absorption
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
		//水平射线，均匀大气层，没有悬浮颗粒(Mie)
		SetUniformAtmosphere();
		RemoveAerosols();
		ExpectNear(
			exp(-kRayleighScattering * sqrt(kTopRadius * kTopRadius - r * r)),
			ComputeTransmittanceToTopAtmosphereBoundary(
				atmosphere_parameters_, r, 0.0)[0],
			Number(kEpsilon));
	}

	/*
	纹理坐标：检查对于大小为n的纹理，纹素0的中心（在纹素坐标0.5 / n处）被映射到0
	并且纹素的中心为n-1（在纹素坐标处（ n-0.5）/ n）映射到1（反之亦然）。 
	最后，检查映射函数及其逆实际上是否相反（即它们的组成(相乘)应该给出Identiy函数）。
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
	映射到透射率纹理坐标：检测r的边界值(r_bottom和r_top)和mu的边界值(mu_horiz和1)是否被映射到透视率纹理的边界texels
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
	这就是单元测试！ 我们只需要实现上面使用的两种方法来设置均匀密度的空气分子和气溶胶，并去除气溶胶：
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





