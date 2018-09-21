#pragma once

/*
这个头文件定义用在SkyFuntion.hlsl里的大气层模型的物理类型和常量，可以被c++编译。
HLSL等价文件SkyDefinitions.hlsl提供一样的类型和常量，允许一样的函数被HLSL编译器编译。

这个C ++编译的主要目的是检查HLSL表达式的维度同质性( dimensional homogeneity)。
为此，我们使用通过物理维度参数化的通用模板来定义C ++物理类型，这些模板受Boost.Unit的启发，并由以下包含的文件提供：
*/
#include "Atmosphere/Constants.h"
#include "Dimensional/angle.h"
#include "Dimensional/binary_function.h"
#include "Dimensional/scalar.h"
#include "Dimensional/scalar_function.h"
#include "Dimensional/ternary_function.h"
#include "Dimensional/vector.h"

namespace Apollo
{
	namespace Atmosphere
	{
		/*
		物理量

		*/
	}
}


