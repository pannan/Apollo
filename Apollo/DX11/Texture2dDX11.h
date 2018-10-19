#pragma once

#include "TextureResource.h"
#include "Vector4.h"

namespace Apollo
{
	class Texture2dDX11 : public TextureResource
	{
	public:

		friend class ShaderParameterDX11;

		Texture2dDX11(const std::string& name);

		Texture2dDX11(const std::string& path, uint32_t handle);

		Texture2dDX11(const std::string& path, uint32_t handle, D3D11_TEXTURE2D_DESC desc, ID3D11Texture2D* texture2d,
			DepthStencilViewComPtr dsv,
			RenderTargetViewComPtr rtv,
			ShaderResourceViewComPtr srv,
			UnorderedAccessViewComPtr uva);
		virtual ~Texture2dDX11();

		void	clear(ClearFlags clearFlags = ClearFlags::All,const Vector4& color = Vector4::ZERO,float depth = 1.0f,uint8_t stencil = 0);
	
		void	bind(UINT slotID,ShaderType shaderType, ShaderParameterType parameterType);

		void unBind(UINT slotID, ShaderType shaderType, ShaderParameterType parameterType);

		void		setTexture2D(ID3D11Texture2D* tex) { m_tex2dDx11 = tex; tex->GetDesc(&m_tex2dDesc); }

		void		setRendertargetView(ID3D11RenderTargetView* rttView);

		void		setDepthStencilView(ID3D11DepthStencilView* depthView) { m_depthStencilViewPtr = depthView; }

		void		setShaderResourceView(ID3D11ShaderResourceView* srv) { m_shaderResourceViewPtr = srv; }

		ID3D11Texture2D*	getTexture2D() { return m_tex2dDx11.Get(); }

		ID3D11DepthStencilView*	getDepthStencilView() { return m_depthStencilViewPtr.Get(); }

		ID3D11RenderTargetView*	getRendertargetView() { return m_renderTargetViewPtr.Get(); }

		ID3D11ShaderResourceView* getShaderResourceView() { return m_shaderResourceViewPtr.Get(); }

	protected:

		DepthStencilViewComPtr			m_depthStencilViewPtr;
		RenderTargetViewComPtr			m_renderTargetViewPtr;	
		ShaderResourceViewComPtr		m_shaderResourceViewPtr;
		UnorderedAccessViewComPtr	m_unorderedAccessViewPtr;

		D3D11_TEXTURE2D_DESC			m_tex2dDesc;
		Texture2DComPtr						m_tex2dDx11;
	};

	typedef std::shared_ptr<Texture2dDX11>	Texture2dDX11Ptr;
}
