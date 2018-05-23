#pragma once

#include "Singleton.h"

namespace Apollo
{
	class DX11Renderer : public SingletonEx<DX11Renderer>
	{
	public:

		DX11Renderer();
		~DX11Renderer();

		HRESULT		init(HWND hWnd);

		void				release();

		void				releaseMainRTT() { SAFE_RELEASE(m_mainRenderTargetView); }

		void				createMainRTT();

		inline ID3D11Device*				getDevice() { return m_pd3dDevice; }

		inline ID3D11DeviceContext* getDeviceContex() { return m_pd3dDeviceContext; }

		inline IDXGISwapChain*			getSwapChain() { return m_pSwapChain; }

		inline ID3D11RenderTargetView*	getMainRTT() { return m_mainRenderTargetView; }

	protected:

		HWND									m_hWnd;
		ID3D11Device*						m_pd3dDevice;
		ID3D11DeviceContext*			m_pd3dDeviceContext;
		IDXGISwapChain*					m_pSwapChain;
		ID3D11RenderTargetView*	m_mainRenderTargetView;

	private:
	};
}
