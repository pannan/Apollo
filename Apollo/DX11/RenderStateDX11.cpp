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

	m_viewports.Width = 1.0f;
	m_viewports.Height = 1.0f;
	m_viewports.TopLeftX = 0.0f;
	m_viewports.TopLeftY = 0.0f;
	m_viewports.MinDepth = 0.0f;
	m_viewports.MaxDepth = 1.0f;
	ID3D11RasterizerState*      m_rasterizerState;
	ID3D11BlendState*				m_blendState;
	FLOAT									m_blendFactor[4];
	UINT									m_sampleMask;
	UINT									m_stencilRef;
	ID3D11DepthStencilState*			m_depthStencilState;
	ID3D11ShaderResourceView*		m_shaderResourceView;
	ID3D11SamplerState*					m_samplerState;
	ID3D11PixelShader*					m_ps;
	ID3D11VertexShader*					m_vs;
	//UINT											PSInstancesCount, VSInstancesCount;
	//ID3D11ClassInstance*        PSInstances[256], *VSInstances[256];   // 256 is max according to PSSetShader documentation
	D3D11_PRIMITIVE_TOPOLOGY    m_primitiveTopology;
	ID3D11Buffer*								m_indexBuffer;
	ID3D11Buffer*								m_vertexBuffer;
	ID3D11Buffer*								m_vsConstantBuffer;
	UINT											m_indexBufferOffset;
	UINT											m_vertexBufferStride;
	UINT											m_vertexBufferOffset;
	DXGI_FORMAT							m_indexBufferFormat;
	ID3D11InputLayout*					m_inputLayout;
}