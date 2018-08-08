#include "stdafx.h"
#include "RendererDX11.h"
#include "Texture2dConfigDX11.h"
#include "TextureDX11ResourceFactory.h"
#include "Texture2dDX11.h"

using namespace Apollo;
using namespace std;

RendererDX11::RendererDX11()
{
	m_hWnd = nullptr;
	m_pd3dDevice = nullptr;
	m_pd3dDeviceContext = nullptr;
	m_pSwapChain = nullptr;
	m_mainRenderTargetView = nullptr;
}

RendererDX11::~RendererDX11()
{
	release();
}

HRESULT RendererDX11::init(HWND hWnd)
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

ID3D11DepthStencilView*	RendererDX11::getMainDepthSteniclView()
{
	Texture2dDX11* depthDX11 = (Texture2dDX11*)(TextureDX11ResourceFactory::getInstance().getResource(m_depthStencilHandle));
	return depthDX11->getDepthStencilView();
}

void RendererDX11::createMainRTT()
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
	
	//create main backbuffer
	m_mainBackBuffer = new Texture2dDX11("Main BackBuffer");
	m_mainBackBuffer->setTexture2D(pBackBuffer);
	m_mainBackBuffer->setRendertargetView(m_mainRenderTargetView);

	pBackBuffer->Release();
}

void RendererDX11::createMainDepthStencil()
{
	//create depth stencil buffer
	ID3D11Resource* pBackBuffer = nullptr;
	m_mainRenderTargetView->GetResource(&pBackBuffer);
	ID3D11Texture2D* dx11BackBuffer = (ID3D11Texture2D*)pBackBuffer;
	D3D11_TEXTURE2D_DESC desc;
	dx11BackBuffer->GetDesc(&desc);
	Texture2dConfigDX11 depthStencilConf;
	depthStencilConf.SetDepthBuffer(desc.Width, desc.Height);
	m_depthStencilHandle = TextureDX11ResourceFactory::getInstance().createTexture2D("MainDepthStencil", depthStencilConf);
	m_pd3dDeviceContext->OMSetRenderTargets(1, &m_mainRenderTargetView, getMainDepthSteniclView());

	//get main depthstencil
	m_mainDepthStencil = (Texture2dDX11*)TextureDX11ResourceFactory::getInstance().getResource(m_depthStencilHandle);
}

void RendererDX11::release()
{
	SAFE_RELEASE(m_mainRenderTargetView);
	SAFE_DELETE(m_mainBackBuffer);
	//SAFE_DELETE(m_mainDepthStencil);由texurefactory负责释放
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

BufferComPtr RendererDX11::createConstantBuffer(const D3D11_BUFFER_DESC& bufferDesc, D3D11_SUBRESOURCE_DATA* pData)
{
	ID3D11Buffer* pBuffer = 0;
	HRESULT hr = m_pd3dDevice->CreateBuffer(&bufferDesc, pData, &pBuffer);
	if (FAILED(hr))
		return(0);

	return(pBuffer);
}

BufferComPtr RendererDX11::createConstantBuffer(UINT size, bool dynamic, bool CPUupdates, D3D11_SUBRESOURCE_DATA* pData)
{
	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = size;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	// Select the appropriate usage and CPU access flags based on the passed
	// in flags
	if (dynamic && CPUupdates)
	{
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else if (dynamic && !CPUupdates)
	{
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.CPUAccessFlags = 0;
	}
	else
	{
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.CPUAccessFlags = 0;
	}
	
	ID3D11Buffer* pBuffer = 0;
	HRESULT hr = m_pd3dDevice->CreateBuffer(&desc, pData, &pBuffer);
	if (FAILED(hr))
	{
		// Handle the error here...
		return(0);
	}
	return(pBuffer);
}

BufferComPtr RendererDX11::createStructuredBuffer(UINT count, UINT structsize, bool CPUWritable, bool GPUWritable, D3D11_SUBRESOURCE_DATA* pData)
{
	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = count * structsize;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = structsize;
	// Select the appropriate usage and CPU access flags based on the passed in flags
	if (!CPUWritable && !GPUWritable)
	{
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.CPUAccessFlags = 0;
	}
	else if (CPUWritable && !GPUWritable)
	{
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else if (!CPUWritable && GPUWritable)
	{
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.CPUAccessFlags = 0;
	}
	else if (CPUWritable && GPUWritable)
	{
		// Handle the error here...
		// Resources can't be writable by both CPU and GPU simultaneously!
	}
	// Create the buffer with the specified configuration
	ID3D11Buffer* pBuffer = 0;
	HRESULT hr = m_pd3dDevice->CreateBuffer(&desc, pData, &pBuffer);
	if (FAILED(hr))
	{
		// Handle the error here...
		return(0);
	}
	return(pBuffer);
}

BufferComPtr RendererDX11::createAppendConsumeBuffer(UINT count, UINT structsize, D3D11_SUBRESOURCE_DATA* pData)
{
	return createStructuredBuffer(count, structsize, false, true, pData);
}


BufferComPtr RendererDX11::createIndirectArgsBuffer(UINT size, D3D11_SUBRESOURCE_DATA* pData)
{
	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = size;
	desc.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.StructureByteStride = 0;
	desc.BindFlags = 0;
	desc.CPUAccessFlags = 0;
	// Create the buffer with the specified configuration
	ID3D11Buffer* pBuffer = 0;
	HRESULT hr = m_pd3dDevice->CreateBuffer(&desc, pData, &pBuffer);
	if (FAILED(hr))
	{
		// Handle the error here...
		return(0);
	}
	return(pBuffer);
}

void RendererDX11::clearDebugInfo()
{
	m_drawCallCount = 0;
}

void RendererDX11::drawIndexed(uint32_t indexCount, uint32_t indexStart, uint32_t vertexStart)
{
	++m_drawCallCount;
	m_pd3dDeviceContext->DrawIndexed(indexCount, indexStart, vertexStart);
}