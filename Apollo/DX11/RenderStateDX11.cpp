#include "stdafx.h"
#include "RenderStateDX11.h"
#include "DX11Renderer.h"
using namespace Apollo;

RenderStateDX11::RenderStateDX11()
{
	m_scissorRects.left = 0.0f;
	m_scissorRects.top = 0.0f;
	m_scissorRects.bottom = 1.0f;
	m_scissorRects.right = 1.0f;

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
	DX11Renderer::getInstance().getDevice()->CreateRasterizerState(&rasterizerDesc, &m_rasterizerState);

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
	DX11Renderer::getInstance().getDevice()->CreateBlendState(&blendDesc, &m_blendState);
	
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
	DX11Renderer::getInstance().getDevice()->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);

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
	DX11Renderer::getInstance().getDevice()->CreateSamplerState(&sampleDesc, &m_samplerState);

	
	m_ps = nullptr;
	m_vs = nullptr;
	//UINT											PSInstancesCount, VSInstancesCount;
	//ID3D11ClassInstance*        PSInstances[256], *VSInstances[256];   // 256 is max according to PSSetShader documentation
	m_primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_indexBuffer = nullptr;
	m_vertexBuffer = nullptr;
	m_vsConstantBuffer = nullptr;
	m_indexBufferOffset = 0;
	m_vertexBufferStride = 0;
	m_vertexBufferOffset = 0;
	m_indexBufferFormat = DXGI_FORMAT_R16_UINT;
	m_inputLayout = nullptr;
}

RenderStateDX11::~RenderStateDX11()
{
	SAFE_RELEASE(m_rasterizerState);
	SAFE_RELEASE(m_blendState);
	SAFE_RELEASE(m_depthStencilState);
	SAFE_RELEASE(m_samplerState);
}

void RenderStateDX11::setDefaultRenderState(ID3D11DeviceContext* dc)
{
	dc->RSSetScissorRects(1, &m_scissorRects);
	dc->RSSetViewports(1, &m_viewports);
	dc->RSSetState(m_rasterizerState);
	SAFE_RELEASE(m_rasterizerState);
	dc->OMSetBlendState(m_blendState, m_blendFactor, m_sampleMask);
	SAFE_RELEASE(m_blendState);
	dc->OMSetDepthStencilState(m_depthStencilState, m_stencilRef); 
	SAFE_RELEASE(m_depthStencilState);
	//dc->PSSetShaderResources(0, 1, nullptr);
	dc->PSSetSamplers(0, 1, &m_samplerState);
	SAFE_RELEASE(m_samplerState);
	//dc->PSSetShader(m_ps,nullptr,0);
	//dc->VSSetShader(m_vs,nullptr,0);
	//dc->VSSetConstantBuffers(0, 1, &m_vsConstantBuffer); if (old.VSConstantBuffer) old.VSConstantBuffer->Release();	
	dc->IASetPrimitiveTopology(m_primitiveTopology);
	//dc->IASetIndexBuffer(old.IndexBuffer, old.IndexBufferFormat, old.IndexBufferOffset); if (old.IndexBuffer) old.IndexBuffer->Release();
	//ctx->IASetVertexBuffers(0, 1, &old.VertexBuffer, &old.VertexBufferStride, &old.VertexBufferOffset); if (old.VertexBuffer) old.VertexBuffer->Release();
	//ctx->IASetInputLayout(old.InputLayout); if (old.InputLayout) old.InputLayout->Release();
}