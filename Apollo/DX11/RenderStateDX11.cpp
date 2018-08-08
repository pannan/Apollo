#include "stdafx.h"
#include "RenderStateDX11.h"
#include "RendererDX11.h"
using namespace Apollo;

RenderStateDX11::RenderStateDX11()
{
	init();
}

void RenderStateDX11::init()
{
	m_scissorRects.left = 0.0f;
	m_scissorRects.top = 0.0f;
	m_scissorRects.bottom = 900;
	m_scissorRects.right = 1600;

	m_viewports.Width = 1600;
	m_viewports.Height = 900;
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
	m_blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	m_blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	m_blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	m_blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	m_blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	m_blendState = nullptr;

	m_blendFactor[0] = m_blendFactor[1] = m_blendFactor[2] = m_blendFactor[3] = 1.0f;
	m_sampleMask = 0xffffffff;
	m_stencilRef = 0;

	//create DepthStencilState
	m_depthStencilDesc.DepthEnable = true;
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
	m_samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;// D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	m_samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	m_samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	m_samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	m_samplerDesc.MipLODBias = 0;
	m_samplerDesc.MaxAnisotropy = 1;	//[1,16]
	m_samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	m_samplerDesc.BorderColor[0] = m_samplerDesc.BorderColor[1] = m_samplerDesc.BorderColor[2] = m_samplerDesc.BorderColor[3] = 0.0f;
	m_samplerDesc.MinLOD = 0.0f;
	m_samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	m_samplerState = nullptr;

	m_primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;


	ZeroMemory(&m_rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	m_rasterizerDesc.CullMode = D3D11_CULL_BACK;
	m_rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	m_rasterizerDesc.FrontCounterClockwise = false;

	m_deviceContext = RendererDX11::getInstance().getDeviceContex();
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

void RenderStateDX11::bind()
{
	m_deviceContext->RSSetScissorRects(1, &m_scissorRects);
	m_deviceContext->RSSetViewports(1, &m_viewports);
	m_deviceContext->RSSetState(m_rasterizerState);
	m_deviceContext->OMSetBlendState(m_blendState, m_blendFactor, m_sampleMask);
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, m_stencilRef);
	m_deviceContext->PSSetSamplers(0, 1, &m_samplerState);
	m_deviceContext->IASetPrimitiveTopology(m_primitiveTopology);

	if (m_renderTarget)
		m_renderTarget->bind();
}

void RenderStateDX11::unBind()
{

}

void RenderStateDX11::setShader(ShaderType type, ShaderDX11Ptr& shader)
{
	m_shaderList[(uint8_t)type] = shader;
}

ShaderDX11Ptr& RenderStateDX11::getShader(ShaderType type)
{
	return m_shaderList[(uint8_t)type];
}