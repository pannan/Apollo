#include "stdafx.h"
#include "RenderStateDX11.h"
#include "RendererDX11.h"
using namespace Apollo;

RenderStateDX11::RenderStateDX11()
{
	m_scissorRects.left = 0.0f;
	m_scissorRects.top = 0.0f;
	m_scissorRects.bottom = 762;
	m_scissorRects.right = 1264;

	m_viewports.Width = 1264;
	m_viewports.Height = 762;
	m_viewports.TopLeftX = 0.0f;
	m_viewports.TopLeftY = 0.0f;
	m_viewports.MinDepth = 0.0f;
	m_viewports.MaxDepth = 1.0f;

	m_rasterizerState = nullptr;

	//create BlendState
	ZeroMemory(&m_blendDesc, sizeof(m_blendDesc));
	m_blendDesc.AlphaToCoverageEnable = false;
	m_blendDesc.IndependentBlendEnable = false;
	m_blendDesc.RenderTarget[0].BlendEnable = false;
	m_blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	m_blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	m_blendDesc.RenderTarget[0].BlendOp  = D3D11_BLEND_OP_ADD;
	m_blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	m_blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	m_blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	m_blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	m_blendState = nullptr;
	
	m_blendFactor[0] = m_blendFactor[1] = m_blendFactor[2] = m_blendFactor[3] = 1.0f;
	m_sampleMask = 0xffffffff;
	m_stencilRef = 0;

	//create DepthStencilState
	m_depthStencilDesc.DepthEnable = false;
	m_depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	m_depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	m_depthStencilDesc.StencilEnable = false;
	m_depthStencilDesc.StencilReadMask = 0;
	m_depthStencilDesc.StencilWriteMask = 0;
	m_depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	m_depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	m_depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	m_depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	m_depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	m_depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	m_depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	m_depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	m_blendState = nullptr;
	

	//create sampler 
	m_samplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	m_samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	m_samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	m_samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	m_samplerDesc.MipLODBias = 0;
	m_samplerDesc.MaxAnisotropy = 1;	//[1,16]
	m_samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	m_samplerDesc.BorderColor[0] = m_samplerDesc.BorderColor[1] = m_samplerDesc.BorderColor[2] = m_samplerDesc.BorderColor[3] = 0.0f;
	m_samplerDesc.MinLOD = 0.0f;
	m_samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	m_samplerState = nullptr;
	
	m_primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
}

void RenderStateDX11::createState()
{
	RendererDX11::getInstance().getDevice()->CreateRasterizerState(&m_rasterizerDesc, &m_rasterizerState);
	RendererDX11::getInstance().getDevice()->CreateBlendState(&m_blendDesc, &m_blendState);
	RendererDX11::getInstance().getDevice()->CreateDepthStencilState(&m_depthStencilDesc, &m_depthStencilState);
	RendererDX11::getInstance().getDevice()->CreateSamplerState(&m_samplerDesc, &m_samplerState);
}

RenderStateDX11::~RenderStateDX11()
{
	SAFE_RELEASE(m_rasterizerState);
	SAFE_RELEASE(m_blendState);
	SAFE_RELEASE(m_depthStencilState);
	SAFE_RELEASE(m_samplerState);
}

void RenderStateDX11::setRenderState(ID3D11DeviceContext* dc)
{
	dc->RSSetScissorRects(1, &m_scissorRects);
	dc->RSSetViewports(1, &m_viewports);
	dc->RSSetState(m_rasterizerState);
	dc->OMSetBlendState(m_blendState, m_blendFactor, m_sampleMask);
	dc->OMSetDepthStencilState(m_depthStencilState, m_stencilRef); 
	dc->PSSetSamplers(0, 1, &m_samplerState);
	dc->IASetPrimitiveTopology(m_primitiveTopology);
}