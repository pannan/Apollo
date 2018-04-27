#include "stdafx.h"
#include "SwapChainDX11.h"
//--------------------------------------------------------------------------------
using namespace Apollo;
//--------------------------------------------------------------------------------
SwapChainDX11::SwapChainDX11(Microsoft::WRL::ComPtr<IDXGISwapChain> pSwapChain, ResourcePtr resource)
{
	m_pSwapChain = pSwapChain;
	m_Resource = resource;
}
//--------------------------------------------------------------------------------
SwapChainDX11::~SwapChainDX11()
{
}
//--------------------------------------------------------------------------------
IDXGISwapChain* SwapChainDX11::GetSwapChain()
{
	return(m_pSwapChain.Get());
}
//--------------------------------------------------------------------------------
