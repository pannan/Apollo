#include "stdafx.h"
#include "StructuredBufferDX11.h"
#include "RendererDX11.h"
#include "LogManager.h"

using namespace Apollo;

StructuredBufferDX11::StructuredBufferDX11(UINT count, UINT structsize, bool CPUWritable, bool GPUWritable, D3D11_SUBRESOURCE_DATA* pData) : m_elementCount(count), m_elementSize(structsize)
{
	m_bUAV = (!CPUWritable && GPUWritable);
	m_bufferComPtr = RendererDX11::getInstance().createStructuredBuffer(count, structsize, CPUWritable, GPUWritable, pData);
}

StructuredBufferDX11::~StructuredBufferDX11()
{}

bool StructuredBufferDX11::bind(unsigned int ID, ShaderType shaderType, ShaderParameterType parameterType)
{
	return true;
}

void StructuredBufferDX11::unBind(unsigned int ID, ShaderType shaderType, ShaderParameterType parameterType)
{
	
}
