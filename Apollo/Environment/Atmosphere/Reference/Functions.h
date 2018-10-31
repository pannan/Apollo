#pragma once

/*
该文件为实现我们的大气模型的GLSL函数提供了C ++头文件。
function.cc中提供了C ++“实现”（该文件在定义了它所依赖的宏之后只包含了GLSL文件）。 文档在GLSL文件中提供。
*/

#include "Environment/Atmosphere/Reference/Definitions.h"
#include "Environment/Atmosphere/Constants.h"
//namespace Apollo
//{
//	namespace Atmosphere
//	{
//		namespace Reference
//		{
NAME_SPACE_BEGIN_APOLLO
NAME_SPACE_BEGIN_ATMOSPHERE
NAME_SPACE_BEGIN_REFERENCE

typedef dimensional::vec2	float2;
typedef dimensional::vec3	float3;
typedef dimensional::vec4	float4;

Length DistanceToTopAtmosphereBoundary(
	const AtmosphereParameters& atmosphere, Length r, Number mu);

Length DistanceToBottomAtmosphereBoundary(
	const AtmosphereParameters& atmosphere, Length r, Number mu);

bool RayIntersectsGround(
	const AtmosphereParameters& atmosphere, Length r, Number mu);

Number GetLayerDensity(const DensityProfileLayer& layer, Length altitude);

Number GetProfileDensity(const DensityProfile& profile, Length altitude);

Length ComputeOpticalLengthToTopAtmosphereBoundary(
	const AtmosphereParameters& atmosphere, const DensityProfile& profile,
	Length r, Number mu);

DimensionlessSpectrum ComputeTransmittanceToTopAtmosphereBoundary(
	const AtmosphereParameters& atmosphere, Length r, Number mu);

Number GetTextureCoordFromUnitRange(Number x, int texture_size);

Number GetUnitRangeFromTextureCoord(Number u, int texture_size);

float2 GetTransmittanceTextureUvFromRMu(const AtmosphereParameters& atmosphere,
	Length r, Number mu);

void GetRMuFromTransmittanceTextureUv(const AtmosphereParameters& atmosphere,
	const float2& uv, Length& r, Number& mu);

DimensionlessSpectrum ComputeTransmittanceToTopAtmosphereBoundaryTexture(
	const AtmosphereParameters& atmosphere, const float2& gl_frag_coord);

DimensionlessSpectrum GetTransmittanceToTopAtmosphereBoundary(
	const AtmosphereParameters& atmosphere,
	const TransmittanceTexture& transmittance_texture,
	Length r, Number mu);

DimensionlessSpectrum GetTransmittance(
	const AtmosphereParameters& atmosphere,
	const TransmittanceTexture& transmittance_texture,
	Length r, Number mu, Length d, bool ray_r_mu_intersects_ground);

// Single scattering.

void ComputeSingleScatteringIntegrand(
	const AtmosphereParameters& atmosphere,
	const TransmittanceTexture& transmittance_texture,
	Length r, Number mu, Number mu_s, Number nu, Length d,
	bool ray_r_mu_intersects_ground,
	DimensionlessSpectrum& rayleigh, DimensionlessSpectrum& mie);

Length DistanceToNearestAtmosphereBoundary(
	const AtmosphereParameters& atmosphere, Length r, Number mu,
	bool ray_r_mu_intersects_ground);

void ComputeSingleScattering(
	const AtmosphereParameters& atmosphere,
	const TransmittanceTexture& transmittance_texture,
	Length r, Number mu, Number mu_s, Number nu,
	bool ray_r_mu_intersects_ground,
	IrradianceSpectrum& rayleigh, IrradianceSpectrum& mie);

InverseSolidAngle RayleighPhaseFunction(Number nu);
InverseSolidAngle MiePhaseFunction(Number g, Number nu);

float4 GetScatteringTextureUvwzFromRMuMuSNu(
	const AtmosphereParameters& atmosphere,
	Length r, Number mu, Number mu_s, Number nu,
	bool ray_r_mu_intersects_ground);

void GetRMuMuSNuFromScatteringTextureUvwz(
	const AtmosphereParameters& atmosphere, const float4& uvwz,
	Length& r, Number& mu, Number& mu_s, Number& nu,
	bool& ray_r_mu_intersects_ground);

void ComputeSingleScatteringTexture(const AtmosphereParameters& atmosphere,
	const TransmittanceTexture& transmittance_texture,
	const float3& gl_frag_coord, IrradianceSpectrum& rayleigh,
	IrradianceSpectrum& mie);

template<class T>
T GetScattering(
	const AtmosphereParameters& atmosphere,
	const AbstractScatteringTexture<T>& scattering_texture,
	Length r, Number mu, Number mu_s, Number nu,
	bool ray_r_mu_intersects_ground);

RadianceSpectrum GetScattering(
	const AtmosphereParameters& atmosphere,
	const ReducedScatteringTexture& single_rayleigh_scattering_texture,
	const ReducedScatteringTexture& single_mie_scattering_texture,
	const ScatteringTexture& multiple_scattering_texture,
	Length r, Number mu, Number mu_s, Number nu,
	bool ray_r_mu_intersects_ground,
	int scattering_order);

// Multiple scattering.

RadianceDensitySpectrum ComputeScatteringDensity(
	const AtmosphereParameters& atmosphere,
	const TransmittanceTexture& transmittance_texture,
	const ReducedScatteringTexture& single_rayleigh_scattering_texture,
	const ReducedScatteringTexture& single_mie_scattering_texture,
	const ScatteringTexture& multiple_scattering_texture,
	const IrradianceTexture& irradiance_texture,
	Length r, Number mu, Number mu_s, Number nu,
	int scattering_order);

RadianceSpectrum ComputeMultipleScattering(
	const AtmosphereParameters& atmosphere,
	const TransmittanceTexture& transmittance_texture,
	const ScatteringDensityTexture& scattering_density_texture,
	Length r, Number mu, Number mu_s, Number nu,
	bool ray_r_mu_intersects_ground);

RadianceDensitySpectrum ComputeScatteringDensityTexture(
	const AtmosphereParameters& atmosphere,
	const TransmittanceTexture& transmittance_texture,
	const ReducedScatteringTexture& single_rayleigh_scattering_texture,
	const ReducedScatteringTexture& single_mie_scattering_texture,
	const ScatteringTexture& multiple_scattering_texture,
	const IrradianceTexture& irradiance_texture,
	const float3& gl_frag_coord, int scattering_order);

RadianceSpectrum ComputeMultipleScatteringTexture(
	const AtmosphereParameters& atmosphere,
	const TransmittanceTexture& transmittance_texture,
	const ScatteringDensityTexture& scattering_density_texture,
	const float3& gl_frag_coord, Number& nu);

// Ground irradiance.

IrradianceSpectrum ComputeDirectIrradiance(
	const AtmosphereParameters& atmosphere,
	const TransmittanceTexture& transmittance_texture,
	Length r, Number mu_s);

IrradianceSpectrum ComputeIndirectIrradiance(
	const AtmosphereParameters& atmosphere,
	const ReducedScatteringTexture& single_rayleigh_scattering_texture,
	const ReducedScatteringTexture& single_mie_scattering_texture,
	const ScatteringTexture& multiple_scattering_texture,
	Length r, Number mu_s, int scattering_order);

float2 GetIrradianceTextureUvFromRMuS(const AtmosphereParameters& atmosphere,
	Length r, Number mu_s);

void GetRMuSFromIrradianceTextureUv(const AtmosphereParameters& atmosphere,
	const float2& uv, Length& r, Number& mu_s);

IrradianceSpectrum ComputeDirectIrradianceTexture(
	const AtmosphereParameters& atmosphere,
	const TransmittanceTexture& transmittance_texture,
	const float2& gl_frag_coord);

IrradianceSpectrum ComputeIndirectIrradianceTexture(
	const AtmosphereParameters& atmosphere,
	const ReducedScatteringTexture& single_rayleigh_scattering_texture,
	const ReducedScatteringTexture& single_mie_scattering_texture,
	const ScatteringTexture& multiple_scattering_texture,
	const float2& gl_frag_coord, int scattering_order);

IrradianceSpectrum GetIrradiance(
	const AtmosphereParameters& atmosphere,
	const IrradianceTexture& irradiance_texture,
	Length r, Number mu_s);

// Rendering.

RadianceSpectrum GetSkyRadiance(
	const AtmosphereParameters& atmosphere,
	const TransmittanceTexture& transmittance_texture,
	const ReducedScatteringTexture& scattering_texture,
	const ReducedScatteringTexture& single_mie_scattering_texture,
	Position camera, const Direction& view_ray, Length shadow_length,
	const Direction& sun_direction, DimensionlessSpectrum& transmittance);

RadianceSpectrum GetSkyRadianceToPoint(
	const AtmosphereParameters& atmosphere,
	const TransmittanceTexture& transmittance_texture,
	const ReducedScatteringTexture& scattering_texture,
	const ReducedScatteringTexture& single_mie_scattering_texture,
	Position camera, const Position& point, Length shadow_length,
	const Direction& sun_direction, DimensionlessSpectrum& transmittance);

IrradianceSpectrum GetSunAndSkyIrradiance(
	const AtmosphereParameters& atmosphere,
	const TransmittanceTexture& transmittance_texture,
	const IrradianceTexture& irradiance_texture,
	const Position& point, const Direction& normal,
	const Direction& sun_direction, IrradianceSpectrum& sky_irradiance);

//////////////////////////////////////////////////////////////////////////

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

//我自己的test 函数
RadianceSpectrum		computeSingleScatting(const AtmosphereParameters& atmosphere,Length r, Number mu, Number mu_s,
						Number nu, bool ray_r_mu_intersects_ground);

RadianceSpectrum		recomputeSingleScatting(const AtmosphereParameters& atmosphere, Length r, Number mu, Number mu_s,
			Number nu, bool ray_r_mu_intersects_ground,
	LazyTransmittanceTexture&	transmittanceTexture,
	LazySingleScatteringTexture&	rayleighSingleScattingTexture,
	LazySingleScatteringTexture& mieSingleScattingTexture);

RadianceSpectrum		getSkyScatting(const AtmosphereParameters& atmosphere, Length r, Number mu, Number mu_s,
	Number nu, bool ray_r_mu_intersects_ground, ReducedScatteringTexture& scattering_texture,
	ReducedScatteringTexture& single_mie_scattering_texture);

IrradianceSpectrum		lookupScatteringTexture(ReducedScatteringTexture& scattering_texture, float3 uvw);

void		computeSingleScattingIntegrand(const AtmosphereParameters& atmosphere,Length r, Number mu, Number mu_s, Number nu, 
	Length d, bool ray_r_mu_intersects_ground,DimensionlessSpectrum& outRayleigh, DimensionlessSpectrum& outMie);

DimensionlessSpectrum		getTransmittance(const AtmosphereParameters& atmosphere,Length r, Number mu, Length d, bool ray_r_mu_intersects_ground);

DimensionlessSpectrum		getTransmittanceToSun(const AtmosphereParameters& atmosphere,Length r, Number mu_s);

bool				testRMuMusNuConversion(AtmosphereParameters& atmosphere,Length r, Number mu, Number mu_s,Number nu, bool ray_r_mu_intersects_ground);



//		}
//	}
//}
NAME_SPACE_END
NAME_SPACE_END
NAME_SPACE_END
