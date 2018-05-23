#include "stdafx.h"
#include "RenderStateDX11.h"
#include "RendererDX11.h"
using namespace Apollo;

RenderStateDX11::RenderStateDX11()
{
	m_scissorRects.left = 0.0f;
	m_scissorRects.top = 0.0f;
	m_scissorRects.bottom = 1.0f;
	m_scissorRects.top = 1.0f;

	m_viewports.Width = 1.0f;
	m_viewports.Height = 1.0f;
	m_viewports.TopLeftX = 0.0f;
	m_viewports.TopLeftY = 0.0f;
	m_viewports.MinDepth = 0.0f;
	m_viewports.MaxDepth = 1.0f;

	//create rasterizerState
	D3D11_RASTERIZER_DESC rasterizerDesc;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.FrontCounterClockwise = true;
	rasterizerDesc.DepthBias = 0.0f;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;
	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.ScissorEnable = true;
	rasterizerDesc.MultisampleEnable = false;
	rasterizerDesc.AntialiasedLineEnable = false;
	RendererDX11::getInstance().GetDevice()->CreateRasterizerState(&rasterizerDesc, &m_rasterizerState);

	//create BlendState
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;
	blendDesc.RenderTarget[0].BlendEnable = false;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp  = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	RendererDX11::getInstance().GetDevice()->CreateBlendState(&blendDesc, &m_blendState);
	
	m_blendFactor[0] = m_blendFactor[1] = m_blendFactor[2] = m_blendFactor[3] = 1.0f;
	m_sampleMask = 0xffffffff;
	m_stencilRef = 0;

	//create DepthStencilState
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = false;
	depthStencilDesc.StencilReadMask = 0;
	depthStencilDesc.StencilWriteMask = 0;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	RendererDX11::getInstance().GetDevice()->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);

	//create 
	D3D11_SAMPLER_DESC sampleDesc;
	sampleDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	sampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.MipLODBias = 0;
	sampleDesc.MaxAnisotropy = 1;	//[1,16]
	sampleDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	sampleDesc.BorderColor[0] = sampleDesc.BorderColor[1] = sampleDesc.BorderColor[2] = sampleDesc.BorderColor[3] = 0.0f;
	sampleDesc.MinLOD = 0.0f;
	sampleDesc.MaxLOD = D3D11_FLOAT32_MAX;
	RendererDX11::getInstance().GetDevice()->CreateSamplerState(&sampleDesc, &m_samplerState);

	
	m_ps = nullptr;
	m_vs = nullptr;
	//UINT											PSInstancesCount, VSInstancesCount;
	//ID3D11ClassInstance*        PSInstances[256], *VSInstances[256];   // 256 is max according to PSSetShader documentation
	m_primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	ID3D11Buffer*								m_indexBuffer;
	ID3D11Buffer*								m_vertexBuffer;
	ID3D11Buffer*								m_vsConstantBuffer;
	UINT											m_indexBufferOffset;
	UINT											m_vertexBufferStride;
	UINT											m_vertexBufferOffset;
	DXGI_FORMAT							m_indexBufferFormat;
	ID3D11InputLayout*					m_inputLayout;
}