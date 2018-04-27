#pragma once

#include "RendererDX11.h"
//--------------------------------------------------------------------------------
namespace Apollo
{
	class SwapChainDX11
	{
	public:
		explicit SwapChainDX11(Microsoft::WRL::ComPtr<IDXGISwapChain> pSwapChain, ResourcePtr resource);
		virtual ~SwapChainDX11();

		IDXGISwapChain* GetSwapChain();

	protected:
		Microsoft::WRL::ComPtr<IDXGISwapChain>	m_pSwapChain;
		ResourcePtr														m_Resource;

		friend RendererDX11;
	};
};
