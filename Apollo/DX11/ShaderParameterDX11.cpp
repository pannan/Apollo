#include "stdafx.h"
#include "ShaderParameterDX11.h"

using namespace Apollo;
using namespace std;

ShaderParameterDX11::ShaderParameterDX11()
	: m_slotID(UINT_MAX)
	, m_parameterType(ShaderParameterType::Invalid)
{}

ShaderParameterDX11::ShaderParameterDX11(const std::string& name, UINT slotID, ShaderType shaderType, ShaderParameterType parameterType)
	: m_Name(name)
	, m_slotID(slotID)
	, m_shaderType(shaderType)
	, m_parameterType(parameterType)
{}

void ShaderParameterDX11::bind()
{
	if (m_texture2dDX11Ptr.get())
		m_texture2dDX11Ptr->bind(m_slotID,m_shaderType,m_parameterType);

	if (m_constantBufferPtr)
		m_constantBufferPtr->bind(m_slotID, m_shaderType, m_parameterType);

	if (m_structuredBufferPtr)
		m_structuredBufferPtr->bind(m_slotID, m_shaderType, m_parameterType);
}

void ShaderParameterDX11::unBind()
{
	if (m_texture2dDX11Ptr.get())
		m_texture2dDX11Ptr->unBind(m_slotID, m_shaderType, m_parameterType);

	if (m_constantBufferPtr)
		m_constantBufferPtr->unBind(m_slotID, m_shaderType, m_parameterType);

	if (m_structuredBufferPtr)
		m_structuredBufferPtr->unBind(m_slotID, m_shaderType, m_parameterType);
}