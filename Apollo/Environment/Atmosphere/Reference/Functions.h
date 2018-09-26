#pragma once

/*
���ļ�Ϊʵ�����ǵĴ���ģ�͵�GLSL�����ṩ��C ++ͷ�ļ���
function.cc���ṩ��C ++��ʵ�֡������ļ��ڶ��������������ĺ�֮��ֻ������GLSL�ļ����� �ĵ���GLSL�ļ����ṩ��
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

			//͸����
			Length DistanceToTopAtmosphereBoundary(const AtmosphereParameters& atmosphere, Length r, Number mu);
		}
	}
}
