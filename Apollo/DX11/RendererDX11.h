#pragma once

#include "Singleton.h"
#include "ResourceProxyDX11.h"

namespace Apollo
{
	class SwapChainConfigDX11;
	class ResourceDX11;
	class SwapChainDX11;

	typedef Microsoft::WRL::ComPtr<ID3D11DeviceContext> DeviceContextComPtr;

	typedef Microsoft::WRL::ComPtr<ID3D11Buffer> BufferComPtr;
	typedef Microsoft::WRL::ComPtr<ID3D11Texture1D> Texture1DComPtr;
	typedef Microsoft::WRL::ComPtr<ID3D11Texture2D> Texture2DComPtr;
	typedef Microsoft::WRL::ComPtr<ID3D11Texture3D> Texture3DComPtr;

	enum ResourceType
	{
		RT_VERTEXBUFFER = 0x010000,
		RT_INDEXBUFFER = 0x020000,
		RT_CONSTANTBUFFER = 0x030000,
		RT_STRUCTUREDBUFFER = 0x040000,
		RT_BYTEADDRESSBUFFER = 0x050000,
		RT_INDIRECTARGSBUFFER = 0x060000,
		RT_TEXTURE1D = 0x070000,
		RT_TEXTURE2D = 0x080000,
		RT_TEXTURE3D = 0x090000
	};

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


		//创建各种buffer方法都是通过ID3D11Buffer接口，只是falg不同
		//ResourcePtr CreateVertexBuffer(BufferConfigDX11* pConfig, D3D11_SUBRESOURCE_DATA* pData);
		//ResourcePtr CreateIndexBuffer(BufferConfigDX11* pConfig, D3D11_SUBRESOURCE_DATA* pData);
		//ResourcePtr CreateConstantBuffer(BufferConfigDX11* pConfig, D3D11_SUBRESOURCE_DATA* pData, bool bAutoUpdate = true);
		//ResourcePtr CreateStructuredBuffer(BufferConfigDX11* pConfig, D3D11_SUBRESOURCE_DATA* pData);
		//ResourcePtr CreateByteAddressBuffer(BufferConfigDX11* pConfig, D3D11_SUBRESOURCE_DATA* pData);
		//ResourcePtr CreateIndirectArgsBuffer(BufferConfigDX11* pConfig, D3D11_SUBRESOURCE_DATA* pData);

		////创建各种纹理
		//ResourcePtr CreateTexture1D(Texture1dConfigDX11* pConfig, D3D11_SUBRESOURCE_DATA* pData,
		//	ShaderResourceViewConfigDX11* pSRVConfig = NULL,
		//	RenderTargetViewConfigDX11* pRTVConfig = NULL,
		//	UnorderedAccessViewConfigDX11* pUAVConfig = NULL);
		//ResourcePtr CreateTexture2D(Texture2dConfigDX11* pConfig, D3D11_SUBRESOURCE_DATA* pData,
		//	ShaderResourceViewConfigDX11* pSRVConfig = NULL,
		//	RenderTargetViewConfigDX11* pRTVConfig = NULL,
		//	UnorderedAccessViewConfigDX11* pUAVConfig = NULL,
		//	DepthStencilViewConfigDX11* pDSVConfig = NULL);
		//ResourcePtr CreateTexture3D(Texture3dConfigDX11* pConfig, D3D11_SUBRESOURCE_DATA* pData,
		//	ShaderResourceViewConfigDX11* pSRVConfig = NULL,
		//	RenderTargetViewConfigDX11* pRTVConfig = NULL,
		//	UnorderedAccessViewConfigDX11* pUAVConfig = NULL);

		// Allow the application to create swap chains
		int CreateSwapChain(SwapChainConfigDX11* pConfig);// ResourceManagerDX11

		ResourcePtr GetSwapChainResource(int ID);
														 
		// This method is here for allowing easy integration with other libraries
		// which require access to the device.  Do not use this interface to create 
		// objects unless those objects are then registered with this renderer class!!!
		// NOTE: This method is likely to be removed in the near future!

		ID3D11Device* GetDevice();

	protected:

		int	StoreNewResource(ResourceDX11* pResource);

		int	GetUnusedResourceIndex();

	protected:

		// The main API interfaces used in the renderer.
		Microsoft::WRL::ComPtr<ID3D11Device>		m_pDevice;
		Microsoft::WRL::ComPtr<ID3D11Debug>		m_pDebugger;
		D3D_DRIVER_TYPE											m_driverType;


		std::vector<ResourceDX11*>							m_vResources;
		// In general, all resources and API objects are housed in expandable arrays
		// wrapper objects.  The position within the array is used to provide fast
		// random access to the renderer clients.

		std::vector<SwapChainDX11*>				m_vSwapChains;
	};
}
