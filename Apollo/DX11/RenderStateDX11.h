#pragma once

#include "ShaderDX11.h"
#include "RenderTargetDX11.h"
#include "Camera.h"

namespace Apollo
{

	class RenderStateDX11
	{
	public:

		RenderStateDX11();

		~RenderStateDX11();

		virtual void		bind();

		virtual void		unBind();

		void		createState();

		void		setShader(ShaderType type, ShaderDX11Ptr& shader);

		ShaderDX11Ptr&		getShader(ShaderType type);

		void		setRenderTarget(RenderTargetDX11Ptr& rtt) { m_renderTarget = rtt; }

		RenderTargetDX11Ptr&		getRenderTarget() { return m_renderTarget; }

		void		setCamera(CameraPtr& cam) { m_camera = cam; }

		CameraPtr&	 getCamera() { return m_camera; }

		void		setRenderPipelineType(RenderPipelineType type) { m_renderPipelineType = type; }

		void		setCullingMode(D3D11_CULL_MODE cullMode) { m_rasterizerDesc.CullMode = cullMode; }

		void		setDepthEnable(bool enable) { m_depthStencilDesc.DepthEnable = enable; }
		void		setDepthWriteMask(D3D11_DEPTH_WRITE_MASK mask) { m_depthStencilDesc.DepthWriteMask = mask; }


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

	protected:

		void		init();

	protected:

		ID3D11DeviceContext*		m_deviceContext;

		ShaderDX11Ptr				m_shaderList[(uint8_t)ShaderType::ShaderTypeCount];

		RenderTargetDX11Ptr		m_renderTarget;

		CameraPtr						m_camera;

		RenderPipelineType		m_renderPipelineType;

	};
}
