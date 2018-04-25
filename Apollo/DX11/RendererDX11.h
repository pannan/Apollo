#pragma once

#include "Singleton.h"

namespace Apollo
{

	typedef Microsoft::WRL::ComPtr<ID3D11DeviceContext> DeviceContextComPtr;

	class RendererDX11 : public SingletonEx<RendererDX11>
	{
	public:
		RendererDX11();
		~RendererDX11();

		
		bool Initialize(D3D_DRIVER_TYPE DriverType, D3D_FEATURE_LEVEL FeatureLevel);
		void Shutdown();

		// These methods provide rendering frame control.  They are closely
		// related to the API for sequencing rendering batches.

		void Present(HWND hWnd = 0, int SwapChain = -1, UINT SyncInterval = 0, UINT PresentFlags = 0);

		// Allow the application to create swap chains

		//int CreateSwapChain(SwapChainConfigDX11* pConfig);// ResourceManagerDX11

														 
		// This method is here for allowing easy integration with other libraries
		// which require access to the device.  Do not use this interface to create 
		// objects unless those objects are then registered with this renderer class!!!
		// NOTE: This method is likely to be removed in the near future!

		ID3D11Device* GetDevice();

	protected:

		// The main API interfaces used in the renderer.
		Microsoft::WRL::ComPtr<ID3D11Device>		m_pDevice;
		Microsoft::WRL::ComPtr<ID3D11Debug>		m_pDebugger;
		D3D_DRIVER_TYPE											m_driverType;


		// In general, all resources and API objects are housed in expandable arrays
		// wrapper objects.  The position within the array is used to provide fast
		// random access to the renderer clients.

		//std::vector<SwapChainDX11*>				m_vSwapChains;
	};
}
