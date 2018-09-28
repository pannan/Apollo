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
#include "Environment/Atmosphere/Functions.hlsl"
		}
	}
}