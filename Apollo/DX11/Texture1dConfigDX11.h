#pragma once

#include "RendererDX11.h"
//--------------------------------------------------------------------------------
namespace Apollo
{
	class Texture1dConfigDX11
	{
	public:
		Texture1dConfigDX11();
		virtual ~Texture1dConfigDX11();

		void SetDefaults();

		void SetWidth(UINT state);
		void SetMipLevels(UINT state);
		void SetArraySize(UINT state);
		void SetFormat(DXGI_FORMAT state);
		void SetUsage(D3D11_USAGE state);
		void SetBindFlags(UINT state);
		void SetCPUAccessFlags(UINT state);
		void SetMiscFlags(UINT state);

		D3D11_TEXTURE1D_DESC GetTextureDesc();

	protected:
		D3D11_TEXTURE1D_DESC 		m_State;

		friend RendererDX11;
	};
};