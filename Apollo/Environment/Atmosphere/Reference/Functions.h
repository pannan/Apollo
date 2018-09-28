#pragma once

/*
该文件为实现我们的大气模型的GLSL函数提供了C ++头文件。
function.cc中提供了C ++“实现”（该文件在定义了它所依赖的宏之后只包含了GLSL文件）。 文档在GLSL文件中提供。
*/

#include "Environment/Atmosphere/Reference/Definitions.h"

namespace Apollo
{
	namespace Atmosphere
	{
		namespace Reference
		{
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
		}
	}
}
