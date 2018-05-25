#include "stdafx.h"
#include "BufferManagerDX11.h"
#include "RendererDX11.h"

// Microsoft::WRL::ComPtr;
using namespace Apollo;
using namespace std;


BufferComPtr BufferManagerDX11::createBuffer(D3D11_BUFFER_DESC* pConfig, D3D11_SUBRESOURCE_DATA* pData)
{
	ID3D11Buffer* buffer = nullptr;

	RendererDX11::getInstance().getDevice()->CreateBuffer(pConfig, pData, &buffer);

	BufferComPtr ptr(buffer);

	m_bufferList.push_back(ptr);

	return ptr;
}