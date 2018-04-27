#pragma once

#include "RendererDX11.h"
//--------------------------------------------------------------------------------
namespace Apollo
{
	class UnorderedAccessViewConfigDX11
	{
	public:
		UnorderedAccessViewConfigDX11();
		virtual ~UnorderedAccessViewConfigDX11();

		void SetDefaults();

		void SetFormat(DXGI_FORMAT state);
		void SetViewDimensions(D3D11_UAV_DIMENSION state);

		void SetBuffer(D3D11_BUFFER_UAV state);
		void SetTexture1D(D3D11_TEX1D_UAV state);
		void SetTexture1DArray(D3D11_TEX1D_ARRAY_UAV state);
		void SetTexture2D(D3D11_TEX2D_UAV state);
		void SetTexture2DArray(D3D11_TEX2D_ARRAY_UAV state);
		void SetTexture3D(D3D11_TEX3D_UAV state);

		D3D11_UNORDERED_ACCESS_VIEW_DESC& GetUAVDesc();

	protected:
		D3D11_UNORDERED_ACCESS_VIEW_DESC    m_State;

		friend RendererDX11;
	};
};
