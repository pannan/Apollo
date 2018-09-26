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

			//透射率
			Length DistanceToTopAtmosphereBoundary(const AtmosphereParameters& atmosphere, Length r, Number mu);
		}
	}
}
