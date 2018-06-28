#pragma once

#include "TextureResource.h"

namespace Apollo
{
	class Texture2dDX11 : public TextureResource
	{
	public:

		Texture2dDX11(const std::string& path, uint32_t handle, ShaderResourceViewComPtr srv);

		Texture2dDX11(const std::string& path, uint32_t handle, ShaderResourceViewComPtr srv,
			DepthStencilViewComPtr dsv,
			RenderTargetViewComPtr rtv,
			UnorderedAccessViewComPtr uav);
		virtual ~Texture2dDX11();

		ID3D11ShaderResourceView*		getSRV() { return m_shaderResourceViewPtr.Get(); }

		ID3D11UnorderedAccessView*	getUAV() { return m_unorderedAccessViewPtr.Get(); }
	
		void	bind(UINT slotID,ShaderType shaderType, ShaderParameterType parameterType);

		void unBind(UINT slotID, ShaderType shaderType, ShaderParameterType parameterType);

	protected:

		ShaderResourceViewComPtr		m_shaderResourceViewPtr;
		DepthStencilViewComPtr			m_depthStencilViewPtr;;
		RenderTargetViewComPtr			m_renderTargetViewPtr;;
		UnorderedAccessViewComPtr	m_unorderedAccessViewPtr;
	};

	typedef std::shared_ptr<Texture2dDX11>	Texture2dDX11Ptr;
}
