#include "stdafx.h"
#include "DX11Renderer.h"

using namespace Apollo;
using namespace std;

DX11Renderer::DX11Renderer()
{
	m_hWnd = nullptr;
	m_pd3dDevice = nullptr;
	m_pd3dDeviceContext = nullptr;
	m_pSwapChain = nullptr;
	m_mainRenderTargetView = nullptr;
}

DX11Renderer::~DX11Renderer()
{
	release();
}

HRESULT DX11Renderer::init(HWND hWnd)
{
	m_hWnd = hWnd;
	// Setup swap chain
	DXGI_SWAP_CHAIN_DESC sd;
	{
		ZeroMemory(&sd, sizeof(sd));
		sd.BufferCount = 2;
		sd.BufferDesc.Width = 0;
		sd.BufferDesc.Height = 0;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = hWnd;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.Windowed = TRUE;
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	}

	UINT createDeviceFlags = 0;
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[1] = { D3D_FEATURE_LEVEL_11_0, };
	if (D3D11CreateDeviceAndSwapChain(	NULL, 
																	D3D_DRIVER_TYPE_HARDWARE, 
																	NULL,
																	createDeviceFlags,
																	featureLevelArray, 
																	1, 
																	D3D11_SDK_VERSION, 
																	&sd,
																	&m_pSwapChain, 
																	&m_pd3dDevice, 
																	&featureLevel, 
																	&m_pd3dDeviceContext) != S_OK)
		return E_FAIL;

	// Setup rasterizer
	{
		D3D11_RASTERIZER_DESC RSDesc;
		memset(&RSDesc, 0, sizeof(D3D11_RASTERIZER_DESC));
		RSDesc.FillMode = D3D11_FILL_SOLID;
		RSDesc.CullMode = D3D11_CULL_NONE;
		RSDesc.FrontCounterClockwise = FALSE;
		RSDesc.DepthBias = 0;
		RSDesc.SlopeScaledDepthBias = 0.0f;
		RSDesc.DepthBiasClamp = 0;
		RSDesc.DepthClipEnable = TRUE;
		RSDesc.ScissorEnable = TRUE;
		RSDesc.AntialiasedLineEnable = FALSE;
		RSDesc.MultisampleEnable = (sd.SampleDesc.Count > 1) ? TRUE : FALSE;

		ID3D11RasterizerState* pRState = NULL;
		m_pd3dDevice->CreateRasterizerState(&RSDesc, &pRState);
		m_pd3dDeviceContext->RSSetState(pRState);
		pRState->Release();
	}

	createMainRTT();

	return S_OK;
}

void DX11Renderer::createMainRTT()
{
	//create main rtt
	DXGI_SWAP_CHAIN_DESC sd;
	m_pSwapChain->GetDesc(&sd);
	// Create the render target
	ID3D11Texture2D* pBackBuffer;
	D3D11_RENDER_TARGET_VIEW_DESC render_target_view_desc;
	ZeroMemory(&render_target_view_desc, sizeof(render_target_view_desc));
	render_target_view_desc.Format = sd.BufferDesc.Format;
	render_target_view_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	m_pd3dDevice->CreateRenderTargetView(pBackBuffer, &render_target_view_desc, &m_mainRenderTargetView);
	m_pd3dDeviceContext->OMSetRenderTargets(1, &m_mainRenderTargetView, NULL);
	pBackBuffer->Release();
}

void DX11Renderer::release()
{
	SAFE_RELEASE(m_mainRenderTargetView);
	SAFE_RELEASE(m_pSwapChain);
	SAFE_RELEASE(m_pd3dDeviceContext);

#if defined(DEBUG) || defined(_DEBUG)  
	ID3D11Debug *d3dDebug;
	HRESULT hr = m_pd3dDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&d3dDebug));
	if (SUCCEEDED(hr))
	{
		hr = d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	}
	SAFE_RELEASE(d3dDebug);
#endif  
	
	SAFE_RELEASE(m_pd3dDevice);
}