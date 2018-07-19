#pragma once

#include "TextureResource.h"

namespace Apollo
{
	class Texture2dDX11 : public TextureResource
	{
	public:

		friend class ShaderParameterDX11;

		Texture2dDX11(const std::string& path, uint32_t handle);

		Texture2dDX11(const std::string& path, uint32_t handle, D3D11_TEXTURE2D_DESC desc, ID3D11Texture2D* texture2d,
			DepthStencilViewComPtr dsv,
			RenderTargetViewComPtr rtv);
		virtual ~Texture2dDX11();
	
		void	bind(UINT slotID,ShaderType shaderType, ShaderParameterType parameterType);

		void unBind(UINT slotID, ShaderType shaderType, ShaderParameterType parameterType);

		void		setTexture2D(ID3D11Texture2D* tex) { m_tex2dDx11 = tex; tex->GetDesc(&m_tex2dDesc); }

		ID3D11Texture2D*	getTexture2D() { return m_tex2dDx11.Get(); }

		ID3D11DepthStencilView*	getDepthStencilView() { return m_depthStencilViewPtr.Get(); }

	protected:

		DepthStencilViewComPtr			m_depthStencilViewPtr;
		RenderTargetViewComPtr			m_renderTargetViewPtr;	

		D3D11_TEXTURE2D_DESC			m_tex2dDesc;
		Texture2DComPtr						m_tex2dDx11;
	};

	typedef std::shared_ptr<Texture2dDX11>	Texture2dDX11Ptr;
}
