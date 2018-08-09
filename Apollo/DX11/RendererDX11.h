#pragma once

#include "Singleton.h"

namespace Apollo
{
	class Texture2dDX11;

	class RendererDX11 : public SingletonEx<RendererDX11>
	{
	public:

		RendererDX11();
		~RendererDX11();

		HRESULT		init(HWND hWnd);

		void				release();

		void				releaseMainRTT() { SAFE_RELEASE(m_mainRenderTargetView); }

		void				createMainRTT();

		void				createMainDepthStencil();

		inline ID3D11Device*				getDevice() { return m_pd3dDevice; }

		inline ID3D11DeviceContext* getDeviceContex() { return m_pd3dDeviceContext; }

		inline IDXGISwapChain*			getSwapChain() { return m_pSwapChain; }

		Texture2dDX11*						getMainRTT() { return m_mainBackBuffer; }

		Texture2dDX11*						getMainDepthBuffer() { return m_mainDepthStencil; }

		inline ID3D11RenderTargetView*	getMainRTTView() { return m_mainRenderTargetView; }

		ID3D11DepthStencilView*				getMainDepthSteniclView();

		BufferComPtr		createConstantBuffer(UINT size,bool dynamic,bool CPUupdates, D3D11_SUBRESOURCE_DATA* pData);

		BufferComPtr		createConstantBuffer(const D3D11_BUFFER_DESC& bufferDesc, D3D11_SUBRESOURCE_DATA* pData);

		BufferComPtr		createStructuredBuffer(UINT count,UINT structsize,bool CPUWritable,bool GPUWritable, D3D11_SUBRESOURCE_DATA* pData);

		BufferComPtr		createAppendConsumeBuffer(UINT count,UINT structsize, D3D11_SUBRESOURCE_DATA* pData);

		BufferComPtr		createIndirectArgsBuffer(UINT size, D3D11_SUBRESOURCE_DATA* pData);

		void						drawIndexed(uint32_t indexCount, uint32_t indexStart, uint32_t vertexStart);

		void						clearDebugInfo();

		int						getBackBufferWidth() { return m_backBufferWidth; }

		int						getBackBufferHeight() { return m_backBufferHeight; }

	protected:

		HWND										m_hWnd;
		ID3D11Device*						m_pd3dDevice;
		ID3D11DeviceContext*			m_pd3dDeviceContext;
		IDXGISwapChain*					m_pSwapChain;
		ID3D11RenderTargetView*		m_mainRenderTargetView;

		uint32_t									m_depthStencilHandle;

		Texture2dDX11*						m_mainBackBuffer;
		Texture2dDX11*						m_mainDepthStencil;

		int											m_backBufferWidth;
		int											m_backBufferHeight;

		//debug info
		uint32_t									m_drawCallCount;

	private:
	};
}
