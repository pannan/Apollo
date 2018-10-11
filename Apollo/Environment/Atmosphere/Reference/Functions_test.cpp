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

	/*
	从透射率纹理坐标的映射：检查透射率纹理的边界纹素的中心是否映射到r（r_bottom和r_top）和mu（mu_horiz和1）的边界值。 
	最后，检查映射函数及其逆实际上是否相反（即它们的乘应该给出Identity函数）。
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
	透射率纹理：检查我们是否获得与顶部大气边界相同的透射率（或多或少于e）
	无论我们是使用ComputeTransmittanceToTopAtmosphereBoundary直接计算，还是通过预先计算的透射率纹理中的双线性插值查找。
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
	透射率纹理：检查GetTransmittance（它在预先计算的透射率纹理中组合了两个双线性插值查找）给出的预期结果
	在某些情况下，可以解析计算结果（当没有气溶胶且空气分子密度均匀时， 光学深度就是Rayleigh散射系数乘以大气中传播的距离。
	*/
	void TestComputeAndGetTransmittance() 
	{
		SetUniformAtmosphere();
		RemoveAerosols();
		LazyTransmittanceTexture transmittance_texture(atmosphere_parameters_);

		const Length r = kBottomRadius * 0.2 + kTopRadius * 0.8;
		const Length d = (kTopRadius - kBottomRadius) * 0.1;

		//水平射线，从底部大气层边界
		ExpectNear(
			exp(-kRayleighScattering * d),
			GetTransmittance(atmosphere_parameters_, transmittance_texture,kBottomRadius, 0.0, d, false /* ray_intersects_ground */)[0],
			Number(kEpsilon));
	
		//向上的，几乎垂直的射线
		ExpectNear(
			exp(-kRayleighScattering * d),
			GetTransmittance(atmosphere_parameters_, transmittance_texture, r, 0.7, d,false /* ray_intersects_ground */)[0],
			Number(kEpsilon));

		//向下的，几乎垂直的射线
		ExpectNear(
			exp(-kRayleighScattering * d),
			GetTransmittance(atmosphere_parameters_, transmittance_texture, r, -0.7, d,RayIntersectsGround(atmosphere_parameters_, r, -0.7))[0],
			Number(kEpsilon));
	}

	/*
	单次散射积分：检查ComputeSingleScatteringIntegrand中的计算（使用预先计算的透射率纹理）给出预期结果，在3种情况下，可以解析计算此结果：

	1)
	垂直射线，从地面向上看，太阳在天顶，散射在r。被积函数这样计算：
		从大气底部到顶部的透射率。
		这涉及从大气的底部到顶部的Rayleigh和Mie光学深度，其具有形式:
		Kext*∫[r_top,r_bottom]exp(-(x-r_bottom)/K)dx = Kext*K [1-exp(-(r_top-r_bottom)/K)]
		在r处的数值密度:exp(-(r-r_bottom)/K)

	2）
	垂直射线，从顶部大气层边界俯视，太阳位于天顶，散射在r。被积函数这样计算：
		从大气顶部到r的透射率，并且返回。
		这涉及从大气顶部到r（乘2）的Rayleigh和Mie光学深度，其形式为：
		Kext*K*[exp(-(r-r_bottom)/K)-exp(-(r_top-r_bottom)/K)]
		在r的数值密度:exp(-(r-r_bottom)/K)

	3)
	从地面的水平射线，太阳在地平线，散射距离观察者的距离d,没有气溶胶和均匀密度的空气分子。
	然后，被积函数只是从地面到顶部大气边界的水平射线的透射率（已经在TestComputeTransmittanceToTopAtmosphereBoundary中计算）。
	*/
	void TestComputeSingleScatteringIntegrand()
	{
		LazyTransmittanceTexture transmittance_texture(atmosphere_parameters_);

		// Vertical ray, from bottom to top atmosphere boundary, scattering at
		// middle of ray, scattering angle equal to 0.
		//从底部到顶部大气层边界的垂直射线，在射线中点散射，nu角度为0
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
		//大气层顶部到中间的垂直射线，nu为180，-1
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
		//从底部到大气层顶部的水平射线,在50km处散射，nu为0，大气均匀，无气溶胶
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
	到最近的大气边界的距离：
	对应垂直向上看(mu = 1)的射线检测值为r_op - r
	水平射线(mu = 0)为sqrt(r_top*r_top - r*r)
	垂直向下看的射线(mu=-1) r-r_bottom
	*/
	void TestDistanceToNearestAtmosphereBoundary()
	{
		constexpr Length r = kBottomRadius * 0.2 + kTopRadius * 0.8;
		// 垂直向上射线
		ExpectNear(
			kTopRadius - r,
			DistanceToNearestAtmosphereBoundary(atmosphere_parameters_, r, 1.0,RayIntersectsGround(atmosphere_parameters_, r, 1.0)),
			1.0 * m);
		// 水平射线
		ExpectNear(
			sqrt(kTopRadius * kTopRadius - r * r),
			DistanceToNearestAtmosphereBoundary(atmosphere_parameters_, r, 0.0,RayIntersectsGround(atmosphere_parameters_, r, 0.0)),
			1.0 * m);
		// 垂直向下射线
		ExpectNear(
			r - kBottomRadius,
			DistanceToNearestAtmosphereBoundary(atmosphere_parameters_, r, -1.0,RayIntersectsGround(atmosphere_parameters_, r, -1.0)),
			1.0 * m);
	}

	/*
	单次散射：检查ComputeSingleScattering中的数值积分是否给出预期结果，在2种情况下，可以解析计算积分：
	1)
	垂直射线，从地面向上看，太阳在天顶。
	单个散射积分具有形式（我们省略了太阳辐照度和Mie术语以简化表达式;另请参见TestComputeSingleScatteringIntegrand）：
	Ksca*∫[r_top,r_bottom] exp(-Kext*K [1 - exp(-(r_top-r_bottom)/K)]) * exp(-(r-r_bottom)/K)dr
	等于
	Ksca * exp(-Kext*K [1 - exp(-(r_top-r_bottom)/K)]) * K [1-exp(-(r_top-r_bottom)K)]

	2)
	垂直射线，从顶部大气边界俯视，太阳在天顶，没有气溶胶。
	单个散射积分具有形式（我们省略了太阳辐照度;另请参见TestComputeSingleScatteringIntegrand）：
	Ksca*∫[r_top,r_bottom] exp(-2*Ksca*K [exp(-(r-r_bottom)/K) - exp(-(r_top-r_bottom)/K)]) * exp(-(r-r_bottom)/K)dr
	其中，令变量u = exp(-(r - r_bottom) / K),给出
	0.5*(1 - exp(-2*Ksca*K(1 - exp(-(r_top-r_bottom) / K))))
	*/
	void TestComputeSingleScattering() 
	{
		LazyTransmittanceTexture transmittance_texture(atmosphere_parameters_);

		// Vertical ray, from bottom atmosphere boundary, scattering angle 0.
		//从下垂直向上到大气层边界，nu = 0
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
	Rayleigh和Mie相位函数：检查这些相位函数在所有立体角上的积分是否为1。
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
	映射到散射纹理坐标：
	检查r(r_top,r_bottom),mu(-1,mu_horiz,1),mu_s(-1,1)和nu(-1,1)的边界值是否映射到中心 散射纹理的边界纹素。
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
	从散射纹理坐标映射：
	检查散射纹理的边界纹素的中心是否映射到r(r_top,t_bottom),mu(-1,mu_horiz,1),mu_s(-1,1),nu(-1,1)
	最后，检查映射函数及其逆实际上是否相反（即它们的组成应该给出标识函数）。
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
	单个散射纹理：
	检查我们是否获得相同的单个散射值（或多或少于e）
	无论我们是使用ComputeSingleScattering直接计算它，还是通过预先计算的单个散射纹理中的quadrilinearly插值查找。
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
	多次散射，步骤1：
	检查ComputeScatteringDensity中的数值积分是否在两种情况下给出预期结果，其中可以解析计算的积分：
	1）
	如果来自(n - 1)阶的入射辐射(radiance)在所有方向上是相同的，并且如果地面反照率是0。
	在这种情况下散射辐射在所有方向上是相同的，并且等于散射系数乘以入射辐射亮度(radiance)
	（相位函数的影响抵消了，因为我们在所有方向上积分，并且相位函数在所有方向上的积分为1）。
	2）
	如果来自(n - 1)阶的入射辐射(radiance)为零，则地面辐照度(irradiance)在任何地方都是相同的，并且透射率为1。
	然后，在地平面上，在水平方向上散射的辐射(radiance)是散射系数乘以地面辐照度(irradiance)，乘以地面反照率，除以π（LambertianBRDF），
	除以2（因为入射辐射(radiance)仅覆盖底部半球，并且由于相位函数的相关性和观察条件 - 尤其是散射辐射(radiance)的水平方向的选择。
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
	多次散射，步骤2：
	检查ComputeMultipleScattering中的数值积分是否可以在某些情况下给出预期结果，其中可以解析计算积分的情况。 
	如果从步骤1计算的辐射度(radiance)在所有方向上都是相同的，并且如果透射率是1，
	那么ComputeMultipleScattering中的数值积分应该简单地等于辐射率乘以距离最近大气边界的距离。 
	我们在下面查看下方的垂直光线，以及观察地平线的光线。
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

//多次散射
//FunctionsTest compute_scattering_density(
//	"ComputeScatteringDensity",
//	&FunctionsTest::TestComputeScatteringDensity);


NAME_SPACE_END
NAME_SPACE_END
NAME_SPACE_END





