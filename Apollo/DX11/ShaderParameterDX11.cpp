#include "stdafx.h"
#include "ShaderParameterDX11.h"

using namespace Apollo;
using namespace std;

ShaderParameterDX11::ShaderParameterDX11()
	: m_slotID(UINT_MAX)
	, m_parameterType(Type::Invalid)
{}

ShaderParameterDX11::ShaderParameterDX11(const std::string& name, UINT slotID, ShaderType shaderType, Type parameterType)
	: m_Name(name)
	, m_slotID(slotID)
	, m_shaderType(shaderType)
	, m_parameterType(parameterType)
{}