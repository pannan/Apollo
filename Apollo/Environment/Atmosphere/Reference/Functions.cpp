/*
���ļ����ṩ�����ǵĴ���ģ�͵�C ++ʵ�֡� 
ʵ���ϣ��ڶ������ܹ�����GLSL�������ΪC ++����ĺ�֮����Ӧ��GLSL�ļ��������ڴ˴������ṩ��ʵ�ʵ�ʵ�֡�
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
#include "Environment/Atmosphere/Functions.hlsl"

			//���Լ���test����
			DimensionlessSpectrum getTransmittance(const AtmosphereParameters& atmosphere,Length r, Number mu, Length d, bool ray_r_mu_intersects_ground)
			{
				Length r_d = ClampRadius(atmosphere, sqrt(d*d + 2 * r*mu*d + r * r));
				Number mu_d = ClampCosine((r * mu + d) / r_d);

				if (ray_r_mu_intersects_ground)
				{
					//DimensionlessSpectrum Tpi = GetTransmittanceToTopAtmosphereBoundary(atmosphere, transmittance_texture, r, -mu);
					//DimensionlessSpectrum Tqi = GetTransmittanceToTopAtmosphereBoundary(atmosphere, transmittance_texture, r_d, -mu_d);
					//DimensionlessSpectrum Tqp = Tqi / Tpi;
					//ע�⣬������Ϊ��mu,mu_dȡ���ˣ�Ϊʲôȡ������Ϊ������費�͵����ཻ
					//�����ҵ��������ʱr_d�����r:r_d->r
					//r�����r_d:r->r_d
					//Ҳ���������Ǵ�r_d�����ģ�����Tpi��Tqi�Ի���,������дΪ
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
				//�ڵ�������һ��p����||op|| = r,
				//��P������������ߣ��е�Ϊe,||oe|| = rbottom = ����뾶
				//oep���ֱ�������Σ�ֱ��Ϊoep,б��Ϊ[o,p]
				//RhΪ[pe]��[p,o�ļн�]
				// sin(Rh) = ||oe|| / ||po|| = rbottom / r	
				Number sin_theta_h = atmosphere.bottom_radius / r;
				Number cos_theta_h = -sqrt(max(1.0 - sin_theta_h * sin_theta_h, 0.0));


				/*
				Rs:̫���춥��
				Rh:��ƽ���춥��
				As:̫���ĽǶȰ뾶����Ҳ��̫���뾶��Ӧ�ĽǶ�
				��ǰ��֪��
				��̫����ȫ�ڵ�ƽ��֮��ʱ����
				Rs > Rh + As  ->
				cos(Rs) < cos(Rh +��s)������cos(Rh) - As*sin(Rh) ->
				cos(Rs) < cos(Rh) - As*sin(Rh) ->
				cos(Rs) - cos(Rh) < -As*sin(Rh)
				Ҳ���ǵ�cos(Rs) - cos(Rh) С�� -As*sin(Rh)ʱ��̫����ȫ�ڵ�ƽ��֮�£�Fraction = 0��
				ͬ��
				��̫����ȫ�ڵ�ƽ��֮��ʱ��
				Rs < Rh - As ->
				cos(Rs) > cos(Rh - As) ->
				cos(Rs) > cos(Rh) + As*sin(Rh) ->
				cos(Rs) - cos(Rh) > As*sin(Rh)
				Ҳ���ǵ�cos(Rs) - cos(Rh) ���� As*sin(Rh)ʱ��̫����ȫ�ڵ�ƽ��֮�ϣ�Fraction = 1��
				Fraction = 0 :cos(Rs) - cos(Rh) < -As*sin(Rh)
				Fraction = 1: cos(Rs) - cos(Rh) > As*sin(Rh)
				������Fraction��<0 �� >1�ķ�Χ��
				Fraction��[0,1]ʱ������ȡ��,�ã�
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

				//q��p��͸����
				DimensionlessSpectrum T_q_p = getTransmittance(atmosphere,r, mu, d, ray_r_mu_intersects_ground);
				//q��̫��֮���͸����
				DimensionlessSpectrum T_sun_q = getTransmittanceToSun(atmosphere,r_d, mu_s_d);

				//̫����q,Ȼ��q��p��͸����
				DimensionlessSpectrum T_sun_q_p = T_sun_q * T_q_p;

				outRayleigh = T_sun_q_p * GetProfileDensity(atmosphere.rayleigh_density, r_d - atmosphere.bottom_radius);
				outMie = T_sun_q_p * GetProfileDensity(atmosphere.mie_density, r_d - atmosphere.bottom_radius);
			}

			RadianceSpectrum computeSingleScatting(const AtmosphereParameters& atmosphere,Length r, Number mu, Number mu_s,
				Number nu, bool ray_r_mu_intersects_ground)
			{
				//��ֵ���ֵļ��������������
				const int SAMPLE_COUNT = 50;
				//������ļ������
				Length dx = DistanceToNearestAtmosphereBoundary(atmosphere, r, mu, ray_r_mu_intersects_ground) / Number(SAMPLE_COUNT);

				DimensionlessSpectrum rayleigh_sum = DimensionlessSpectrum(0.0);
				DimensionlessSpectrum mie_sum = DimensionlessSpectrum(0.0);

				for (int i = 0; i <= SAMPLE_COUNT; ++i)
				{
					Length d_i = Number(i) * dx;

					DimensionlessSpectrum rayleigh_i;
					DimensionlessSpectrum mie_i;

					computeSingleScattingIntegrand(atmosphere, r, mu, mu_s, nu, d_i, ray_r_mu_intersects_ground, rayleigh_i, mie_i);

					//����Ȩ��(trapezoidla rule)
					Number weight_i = (i == 0) || (i == SAMPLE_COUNT) ? 0.5 : 1.0;
					rayleigh_sum += rayleigh_i * weight_i;
					mie_sum += mie_i * weight_i;
				}

				IrradianceSpectrum rayleighIrradiance = rayleigh_sum * dx * atmosphere.solar_irradiance * atmosphere.rayleigh_scattering;
				IrradianceSpectrum mieIrradiance = mie_sum * dx * atmosphere.solar_irradiance * atmosphere.mie_scattering;

				//����λ����
				RadianceSpectrum rayleigh = rayleighIrradiance * RayleighPhaseFunction(nu);
				RadianceSpectrum mie = mieIrradiance * MiePhaseFunction(atmosphere.mie_phase_function_g, nu);

				return rayleigh + mie;
			}

		}
	}
}