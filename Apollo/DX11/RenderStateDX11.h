#pragma once

namespace Apollo
{
	class RenderStateDX11 
	{
	public:

		RenderStateDX11();

		~RenderStateDX11();

		void		setRenderState(ID3D11DeviceContext* dc);

		void		createState();

//	private:

		D3D11_RECT						m_scissorRects;
		D3D11_VIEWPORT              m_viewports;
		ID3D11RasterizerState*      m_rasterizerState;
		ID3D11BlendState*				m_blendState;
		FLOAT									m_blendFactor[4];
		UINT									m_sampleMask;
		UINT									m_stencilRef;
		ID3D11DepthStencilState*			m_depthStencilState;
		ID3D11SamplerState*					m_samplerState;
		D3D11_PRIMITIVE_TOPOLOGY    m_primitiveTopology;	

		D3D11_RASTERIZER_DESC	m_rasterizerDesc;
		D3D11_BLEND_DESC			m_blendDesc;
		D3D11_DEPTH_STENCIL_DESC	m_depthStencilDesc;
		D3D11_SAMPLER_DESC		m_samplerDesc;
	
	};
}
