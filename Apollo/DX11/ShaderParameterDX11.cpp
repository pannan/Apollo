#include "stdafx.h"
#include "ShaderParameterDX11.h"
#include "RendererDX11.h"
#include "LogManager.h"

using namespace Apollo;
using namespace std;

ShaderParameterDX11::ShaderParameterDX11()
	: m_slotID(UINT_MAX)
	, m_parameterType(ShaderParameterType::Invalid)
	, m_texture2dDX11(nullptr)
{}

ShaderParameterDX11::ShaderParameterDX11(const std::string& name, UINT slotID, ShaderType shaderType, ShaderParameterType parameterType)
	: m_Name(name)
	, m_slotID(slotID)
	, m_shaderType(shaderType)
	, m_parameterType(parameterType)
	, m_texture2dDX11(nullptr)
{}

void ShaderParameterDX11::setStructuredBuffer(StructuredBufferDX11Ptr buffer)
{
	m_structuredBufferPtr = buffer;
}

void ShaderParameterDX11::setTexture2d(Texture2dDX11* tex2d)
{
	m_texture2dDX11 = tex2d;
}

void ShaderParameterDX11::bind()
{
	if (m_texture2dDX11)
		m_texture2dDX11->bind(m_slotID, m_shaderType, m_parameterType);	
		

	if (m_constantBufferPtr)
		m_constantBufferPtr->bind(m_slotID, m_shaderType, m_parameterType);

	if (m_structuredBufferPtr)
	{
		m_structuredBufferPtr->bind(m_slotID, m_shaderType, m_parameterType);		
	}		
}

void ShaderParameterDX11::unBind()
{
	if (m_texture2dDX11)
	{
		m_texture2dDX11->unBind(m_slotID, m_shaderType, m_parameterType);
	}		

	if (m_constantBufferPtr)
		m_constantBufferPtr->unBind(m_slotID, m_shaderType, m_parameterType);

	if (m_structuredBufferPtr)
	{
		m_structuredBufferPtr->unBind(m_slotID, m_shaderType, m_parameterType);
	}
}