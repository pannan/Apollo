#include "stdafx.h"
#include "RendererDX11.h"
#include "DXGIAdapter.h"
#include "DXGIOutput.h"
#include "SwapChainConfigDX11.h"
#include "Texture2dDX11.h"
#include "Texture2dConfigDX11.h"
#include "SwapChainDX11.h"
//#include "BufferConfigDX11.h"
//#include "IndexBufferDX11.h"
//#include "StructuredBufferDX11.h"
//#include "ByteAddressBufferDX11.h"
//#include "IndirectArgsBufferDX11.h"

#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "DXGI.lib" )

using Microsoft::WRL::ComPtr;
using namespace Apollo;

bool RendererDX11::Initialize(D3D_DRIVER_TYPE DriverType, D3D_FEATURE_LEVEL FeatureLevel)
{

	HRESULT hr = S_OK;

	// 创建一个factory来枚举系统支持的硬件
	ComPtr<IDXGIFactory1> pFactory;
	hr = CreateDXGIFactory1(__uuidof(IDXGIFactory), reinterpret_cast<void**>(pFactory.GetAddressOf()));


	// 枚举所有显卡，甚至不支持dx11的
	ComPtr<IDXGIAdapter1> pCurrentAdapter;
	std::vector<DXGIAdapter> vAdapters;

	while (pFactory->EnumAdapters1(static_cast<UINT>(vAdapters.size()), pCurrentAdapter.ReleaseAndGetAddressOf()) != DXGI_ERROR_NOT_FOUND)
	{
		vAdapters.push_back(pCurrentAdapter);

		DXGI_ADAPTER_DESC1 desc;
		pCurrentAdapter->GetDesc1(&desc);

	//	Log::Get().Write(desc.Description);
	}

	// Specify debug
	UINT CreateDeviceFlags = 0;
#ifdef _DEBUG
	CreateDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	DeviceContextComPtr pContext;

	D3D_FEATURE_LEVEL level[] = { FeatureLevel };
	D3D_FEATURE_LEVEL CreatedLevel;

	// 遍历所有显卡找到适合的硬件
	if (DriverType == D3D_DRIVER_TYPE_HARDWARE)
	{
		for (auto pAdapter : vAdapters)
		{
			hr = D3D11CreateDevice(
				pAdapter.m_pAdapter.Get(),
				D3D_DRIVER_TYPE_UNKNOWN,
				nullptr,
				CreateDeviceFlags,
				level,
				1,
				D3D11_SDK_VERSION,
				m_pDevice.GetAddressOf(),
				&CreatedLevel,
				pContext.GetAddressOf());

			if (hr == S_OK)
				break;
		}
	}
	else
	{
		hr = D3D11CreateDevice(
			nullptr,
			DriverType,
			nullptr,
			CreateDeviceFlags,
			level,
			1,
			D3D11_SDK_VERSION,
			m_pDevice.GetAddressOf(),
			&CreatedLevel,
			pContext.GetAddressOf());
	}

	if (FAILED(hr))
		return false;

	// Get the debugger interface from the device.

	hr = m_pDevice.CopyTo(m_pDebugger.GetAddressOf());

	if (FAILED(hr))
	{
		//Log::Get().Write(L"Unable to acquire the ID3D11Debug interface from the device!");
	}

	return(true);
}

int	RendererDX11::GetUnusedResourceIndex()
{
	// Initialize return index to -1.
	int index = -1;

	// Search for a NULL index location.
	for (unsigned int i = 0; i < m_vResources.size(); i++) {
		if (m_vResources[i] == NULL) {
			index = i;
			break;
		}
	}

	// Return either an empty location, or -1 if none exist.
	return(index);
}

int	RendererDX11::StoreNewResource(ResourceDX11* pResource)
{
	// This method either finds an empty spot in the list, or just appends the
	// resource to the end of it if none are available.

	int index = GetUnusedResourceIndex();

	if (index == -1) {
		m_vResources.push_back(pResource);
		index = m_vResources.size() - 1;
	}
	else {
		m_vResources[index] = pResource;
	}

	// Shift the inner ID to the upper 16 bits.
	int innerID = (int)pResource->GetInnerID() << 16;
	index = index + innerID;

	return(index);
}

int RendererDX11::CreateSwapChain(SwapChainConfigDX11* pConfig)
{
	// Attempt to create the DXGI Factory.

	ComPtr<IDXGIDevice> pDXGIDevice;
	HRESULT hr = m_pDevice.CopyTo(pDXGIDevice.GetAddressOf());

	ComPtr<IDXGIAdapter> pDXGIAdapter;
	hr = pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void **>(pDXGIAdapter.GetAddressOf()));

	ComPtr<IDXGIFactory> pFactory;
	pDXGIAdapter->GetParent(__uuidof(IDXGIFactory), reinterpret_cast<void **>(pFactory.GetAddressOf()));


	// Attempt to create the swap chain.

	ComPtr<IDXGISwapChain> pSwapChain;
	hr = pFactory->CreateSwapChain(m_pDevice.Get(), &pConfig->m_State, pSwapChain.GetAddressOf());


	// Release the factory regardless of pass or fail.

	if (FAILED(hr))
	{
		//Log::Get().Write(L"Failed to create swap chain!");
		return(-1);
	}


	// Acquire the texture interface from the swap chain.

	Texture2DComPtr pSwapChainBuffer;
	hr = pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(pSwapChainBuffer.GetAddressOf()));

	if (FAILED(hr))
	{
	//	Log::Get().Write(L"Failed to get swap chain texture resource!");
		return(-1);
	}


	// Add the swap chain's back buffer texture and render target views to the internal data
	// structures to allow setting them later on.

	int ResourceID = StoreNewResource(new Texture2dDX11(pSwapChainBuffer));


	// If we get here, then we succeeded in creating our swap chain and it's constituent parts.
	// Now we create the wrapper object and store the result in our container.

	Texture2dConfigDX11 TextureConfig;
	pSwapChainBuffer->GetDesc(&TextureConfig.m_State);

	ResourcePtr Proxy(new ResourceProxyDX11(ResourceID, &TextureConfig, this));
	// With the resource proxy created, create the swap chain wrapper and store it.
	// The resource proxy can then be used later on by the application to get the
	// RTV or texture ID if needed.

	m_vSwapChains.push_back(new SwapChainDX11(pSwapChain, Proxy));

	return(m_vSwapChains.size() - 1);
}

ResourcePtr RendererDX11::GetSwapChainResource(int ID)
{
	unsigned int index = static_cast<unsigned int>(ID);

	if (index < m_vSwapChains.size())
		return(m_vSwapChains[index]->m_Resource);

	//Log::Get().Write(L"Tried to get an invalid swap buffer index texture ID!");

	return(ResourcePtr(new ResourceProxyDX11()));
}