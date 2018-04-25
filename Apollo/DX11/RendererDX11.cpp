#include "stdafx.h"
#include "RendererDX11.h"
#include "DXGIAdapter.h"
#include "DXGIOutput.h"

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
