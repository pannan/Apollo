/*
该文件“提供”我们的大气模型的C ++实现。 
实际上，在定义了能够将此GLSL代码编译为C ++所需的宏之后，相应的GLSL文件（包含在此处）中提供了实际的实现。
*/

#include "stdafx.h"

#include "Environment/Atmosphere/Reference/Functions.h"

#define _IN(x) const x&
#define _OUT(x) x&
#define TEMPLATE(x) template<class x>
#define TEMPLATE_ARGUMENT(x) <x>

namespace Apollo
{
	namespace Atmosphere
	{
		namespace Reference
		{
			using std::max;
			using std::min;
//#include "Environment/Atmosphere/Functions.hlsl"
#include "../bin/Assets/Shader/Functions.hlsl"

			bool MyExpectNear(double expected, double actual, double tolerance) 
			{
				if (actual - expected > tolerance || expected - actual > tolerance) 
				{
					std::cout << expected << " +/- " << tolerance << " expected but got "
						<< actual << std::endl;
					//pass_ = false;
					return false;
				}

				return true;
			}

			bool		testRMuMusNuConversion(AtmosphereParameters& atmosphere, Length r, Number mu, Number mu_s, Number nu, bool ray_r_mu_intersects_ground)
			{
				float4 uvwz = GetScatteringTextureUvwzFromRMuMuSNu(atmosphere,r,mu,mu_s,nu,ray_r_mu_intersects_ground);
				constexpr double kEpsilon = 1e-3;
				Length _r;
				Number _mu;
				Number _mu_s;
				Number _nu;

				//uvwz to uvw
				Number tex_coord_x = uvwz.x * Number(SCATTERING_TEXTURE_NU_SIZE - 1);
				Number tex_x = floor(tex_coord_x);
				Number lerp = tex_coord_x - tex_x;
				float3 uvw0 = float3((tex_x + uvwz.y) / Number(SCATTERING_TEXTURE_NU_SIZE), uvwz.z, uvwz.w);
				float3 uvw1 = float3((tex_x + 1.0 + uvwz.y) / Number(SCATTERING_TEXTURE_NU_SIZE), uvwz.z, uvwz.w);

				GetRMuMuSNuFromScatteringTextureFragCoord(atmosphere, uvw0, _r, _mu, _mu_s, _nu, ray_r_mu_intersects_ground);

				//下面函数测试没问题
				//GetRMuMuSNuFromScatteringTextureUvwz(atmosphere, uvwz, _r, _mu, _mu_s, _nu, ray_r_mu_intersects_ground);

				bool isOk = true;
				isOk &= MyExpectNear(r.to(m), _r.to(m), (kEpsilon) );

				isOk &= MyExpectNear(mu(), _mu(), (kEpsilon));

				isOk &= MyExpectNear(mu_s(), _mu_s(), (kEpsilon));

				isOk &= MyExpectNear(nu(), _nu(), (kEpsilon));

				if (isOk == false)
					std::cout << "testRMuMusNuConversion false";

				return isOk;
			}

			//我自己的test函数
			DimensionlessSpectrum getTransmittance(const AtmosphereParameters& atmosphere,Length r, Number mu, Length d, bool ray_r_mu_intersects_ground)
			{
				Length r_d = ClampRadius(atmosphere, sqrt(d*d + 2 * r*mu*d + r * r));
				Number mu_d = ClampCosine((r * mu + d) / r_d);

				if (ray_r_mu_intersects_ground)
				{
					//DimensionlessSpectrum Tpi = GetTransmittanceToTopAtmosphereBoundary(atmosphere, transmittance_texture, r, -mu);
					//DimensionlessSpectrum Tqi = GetTransmittanceToTopAtmosphereBoundary(atmosphere, transmittance_texture, r_d, -mu_d);
					//DimensionlessSpectrum Tqp = Tqi / Tpi;
					//注意，这里因为对mu,mu_d取反了，为什么取反？因为这里假设不和地面相交
					//这里我的理解是这时r_d变成了r:r_d->r
					//r变成了r_d:r->r_d
					//也就是射线是从r_d发出的，所以Tpi和Tqi对换了,所以重写为
					DimensionlessSpectrum Tqi = ComputeTransmittanceToTopAtmosphereBoundary(atmosphere, r, -mu);
					DimensionlessSpectrum Tpi = ComputeTransmittanceToTopAtmosphereBoundary(atmosphere, r_d, -mu_d);
					DimensionlessSpectrum Tpq = Tpi / Tqi;
					return min(Tpq, DimensionlessSpectrum(1.0));
				}
				else
				{
					DimensionlessSpectrum Tpi = ComputeTransmittanceToTopAtmosphereBoundary(atmosphere, r, mu);
					DimensionlessSpectrum Tqi = ComputeTransmittanceToTopAtmosphereBoundary(atmosphere, r_d, mu_d);
					DimensionlessSpectrum Tpq = Tpi / Tqi;
					return min(Tpq, DimensionlessSpectrum(1.0));
				}
			}

			DimensionlessSpectrum getTransmittanceToSun(const AtmosphereParameters& atmosphere,Length r, Number mu_s)
			{
				//sin_theta_h = sin(Rh)
				//在地面上有一点p，有||op|| = r,
				//从P点向地面做切线，切点为e,||oe|| = rbottom = 地球半径
				//oep组成直角三角形，直角为oep,斜边为[o,p]
				//Rh为[pe]和[p,o的夹角]
				// sin(Rh) = ||oe|| / ||po|| = rbottom / r	
				Number sin_theta_h = atmosphere.bottom_radius / r;
				Number cos_theta_h = -sqrt(max(1.0 - sin_theta_h * sin_theta_h, 0.0));


				/*
				Rs:太阳天顶角
				Rh:地平线天顶角
				As:太阳的角度半径，就也是太阳半径对应的角度
				由前面知：
				当太阳完全在地平线之下时，有
				Rs > Rh + As  ->
				cos(Rs) < cos(Rh +Ａs)　＝～cos(Rh) - As*sin(Rh) ->
				cos(Rs) < cos(Rh) - As*sin(Rh) ->
				cos(Rs) - cos(Rh) < -As*sin(Rh)
				也就是当cos(Rs) - cos(Rh) 小于 -As*sin(Rh)时，太阳完全在地平线之下（Fraction = 0）
				同理：
				当太阳完全在地平线之上时：
				Rs < Rh - As ->
				cos(Rs) > cos(Rh - As) ->
				cos(Rs) > cos(Rh) + As*sin(Rh) ->
				cos(Rs) - cos(Rh) > As*sin(Rh)
				也就是当cos(Rs) - cos(Rh) 大于 As*sin(Rh)时，太阳完全在地平线之上（Fraction = 1）
				Fraction = 0 :cos(Rs) - cos(Rh) < -As*sin(Rh)
				Fraction = 1: cos(Rs) - cos(Rh) > As*sin(Rh)
				上面是Fraction在<0 和 >1的范围，
				Fraction在[0,1]时，符号取反,得：
				-As*sin(Rh) <= cos(Rs) - cos(Rh) <= As*sin(Rh)
				smoothstep(-As*sin(Rh),As*sin(Rh),cos(Rs) - cos(Rh))
				*/
				Number sin_theat_h_sun_angular = sin_theta_h * atmosphere.sun_angular_radius / rad;
				Number Fraction = smoothstep(-sin_theat_h_sun_angular, sin_theat_h_sun_angular, mu_s - cos_theta_h);
				DimensionlessSpectrum TransmittanceToSun = ComputeTransmittanceToTopAtmosphereBoundary(atmosphere, r, mu_s);

				return TransmittanceToSun * Fraction;
			}

			void computeSingleScattingIntegrand(const AtmosphereParameters& atmosphere,Length r, Number mu, Number mu_s, Number nu, 
				Length d, bool ray_r_mu_intersects_ground,
				DimensionlessSpectrum& outRayleigh, DimensionlessSpectrum& outMie)
			{
				Length r_d = ClampRadius(atmosphere, sqrt(d*d + 2 * d*r*mu + r * r));
				Number mu_s_d = ClampCosine((r*mu_s + nu * d) / r_d);

				//q到p的透射率
				DimensionlessSpectrum T_q_p = getTransmittance(atmosphere,r, mu, d, ray_r_mu_intersects_ground);
				//q和太阳之间的透射率
				DimensionlessSpectrum T_sun_q = getTransmittanceToSun(atmosphere,r_d, mu_s_d);

				//太阳到q,然后q到p的透射率
				DimensionlessSpectrum T_sun_q_p = T_sun_q * T_q_p;

				outRayleigh = T_sun_q_p * GetProfileDensity(atmosphere.rayleigh_density, r_d - atmosphere.bottom_radius);
				outMie = T_sun_q_p * GetProfileDensity(atmosphere.mie_density, r_d - atmosphere.bottom_radius);
			}

			RadianceSpectrum computeSingleScatting(const AtmosphereParameters& atmosphere,Length r, Number mu, Number mu_s,
				Number nu, bool ray_r_mu_intersects_ground)
			{
				//数值积分的间隔数（采样数）
				const int SAMPLE_COUNT = 50;
				//采样点的间隔长度
				Length dx = DistanceToNearestAtmosphereBoundary(atmosphere, r, mu, ray_r_mu_intersects_ground) / Number(SAMPLE_COUNT);

				DimensionlessSpectrum rayleigh_sum = DimensionlessSpectrum(0.0);
				DimensionlessSpectrum mie_sum = DimensionlessSpectrum(0.0);

				for (int i = 0; i <= SAMPLE_COUNT; ++i)
				{
					Length d_i = Number(i) * dx;

					DimensionlessSpectrum rayleigh_i;
					DimensionlessSpectrum mie_i;

					computeSingleScattingIntegrand(atmosphere, r, mu, mu_s, nu, d_i, ray_r_mu_intersects_ground, rayleigh_i, mie_i);

					//采样权重(trapezoidla rule)
					Number weight_i = (i == 0) || (i == SAMPLE_COUNT) ? 0.5 : 1.0;
					rayleigh_sum += rayleigh_i * weight_i;
					mie_sum += mie_i * weight_i;
				}

				IrradianceSpectrum rayleighIrradiance = rayleigh_sum * dx * atmosphere.solar_irradiance * atmosphere.rayleigh_scattering;
				IrradianceSpectrum mieIrradiance = mie_sum * dx * atmosphere.solar_irradiance * atmosphere.mie_scattering;

				//乘相位函数
				RadianceSpectrum rayleigh = rayleighIrradiance * RayleighPhaseFunction(nu);
				RadianceSpectrum mie = mieIrradiance * MiePhaseFunction(atmosphere.mie_phase_function_g, nu);

				return rayleigh + mie;
			}

			RadianceSpectrum recomputeSingleScatting(const AtmosphereParameters& atmosphere, Length r, Number mu, Number mu_s,
				Number nu, bool ray_r_mu_intersects_ground,
				LazyTransmittanceTexture&	transmittanceTexture,
				LazySingleScatteringTexture&	rayleighSingleScattingTexture,
				LazySingleScatteringTexture& mieSingleScattingTexture)
			{
				/*LazyTransmittanceTexture transmittance_texture(atmosphere);
				LazySingleScatteringTexture single_rayleigh_scattering_texture(atmosphere, transmittance_texture, true);
				LazySingleScatteringTexture single_mie_scattering_texture(atmosphere, transmittance_texture, false);*/

				//Vertical ray, from bottom atmosphere boundary, scattering angle 0.
				IrradianceSpectrum rayleighIrradiance = GetScattering(atmosphere, rayleighSingleScattingTexture,r, mu, mu_s, nu, false);
				IrradianceSpectrum mieIrradiance = GetScattering(atmosphere, mieSingleScattingTexture,r, mu, mu_s, nu, false);
				
				//乘相位函数
				RadianceSpectrum rayleigh = rayleighIrradiance * RayleighPhaseFunction(nu);
				RadianceSpectrum mie = mieIrradiance * MiePhaseFunction(atmosphere.mie_phase_function_g, nu);

				return rayleigh + mie;
			}

			RadianceSpectrum getSkyScatting(const AtmosphereParameters& atmosphere, Length r, Number mu, Number mu_s,
				Number nu, bool ray_r_mu_intersects_ground,
				ReducedScatteringTexture& scattering_texture, ReducedScatteringTexture& single_mie_scattering_texture)
			{
				IrradianceSpectrum single_mie_scattering;
				IrradianceSpectrum scattering = GetCombinedScattering(
					atmosphere, scattering_texture, single_mie_scattering_texture,
					r, mu, mu_s, nu, ray_r_mu_intersects_ground, single_mie_scattering);

				return scattering * RayleighPhaseFunction(nu) + single_mie_scattering *
					MiePhaseFunction(atmosphere.mie_phase_function_g, nu);
			}

		}
	}
}