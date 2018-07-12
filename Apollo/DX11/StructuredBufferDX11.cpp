#include "stdafx.h"
#include "StructuredBufferDX11.h"
#include "RendererDX11.h"
#include "LogManager.h"

using namespace Apollo;

StructuredBufferDX11::StructuredBufferDX11(UINT count, UINT structsize, bool CPUWritable, bool GPUWritable, D3D11_SUBRESOURCE_DATA* pData) : 
					m_elementCount(count), m_elementSize(structsize),m_CPUWritable(CPUWritable)
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

void StructuredBufferDX11::commit(void* data, uint32_t size)
{
	if (m_CPUWritable == false)
	{
		LogManager::getInstance().log("StructuredBufferDX11::commit m_CPUWritable == false!");
		return;
	}
	
	if (m_bufferComPtr.Get() == nullptr)
	{
		LogManager::getInstance().log("StructuredBufferDX11::commit m_bufferComPtr.Get() == nullptr!");
		return;
	}
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(RendererDX11::getInstance().getDeviceContex()->Map(m_bufferComPtr.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		LogManager::getInstance().log("StructuredBufferDX11::commit Map false!");
		return;
	}

	memcpy_s(mappedResource.pData, m_elementSize * m_elementCount, data, size);

	RendererDX11::getInstance().getDeviceContex()->Unmap(m_bufferComPtr.Get(), 0);
}