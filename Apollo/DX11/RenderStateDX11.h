#pragma once

#include "Singleton.h"
namespace Apollo
{
	class RenderStateDX11 : public SingletonEx<RenderStateDX11>
	{
	public:

		RenderStateDX11();

		~RenderStateDX11();

		void		setDefaultRenderState(ID3D11DeviceContext* dc);

	private:

		D3D11_RECT						m_scissorRects;
		D3D11_VIEWPORT              m_viewports;
		ID3D11RasterizerState*      m_rasterizerState;
		ID3D11BlendState*				m_blendState;
		FLOAT									m_blendFactor[4];
		UINT									m_sampleMask;
		UINT									m_stencilRef;
		ID3D11DepthStencilState*			m_depthStencilState;
		//ID3D11ShaderResourceView*		m_shaderResourceView;
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
	
	};
}
