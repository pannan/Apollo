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

			//Length ComputeOpticalLengthToTopAtmosphereBoundary(_IN(AtmosphereParameters) atmosphere, _IN(DensityProfile) profile, Length r, Number mu)
			//{
			//	//��ֵ���ֵļ������
			//	const int SAMPLE_COUNT = 500;
			//}

			//DimensionlessSpectrum ComputeTransmittanceToTopAtmosphereBoundary(_IN(AtmosphereParameters) atmosphere, Length r, Number mu)
			//{
			//	Length opticalLength_rayleigh = ComputeOpticalLengthToTopAtmosphereBoundary(atmosphere, atmosphere.rayleigh_density, r, mu);
			//}
		}
	}
}