#pragma once

#include "BufferDX11.h"
//--------------------------------------------------------------------------------
namespace Apollo
{
	class IndirectArgsBufferDX11 : public BufferDX11
	{
	public:
		IndirectArgsBufferDX11(Microsoft::WRL::ComPtr<ID3D11Buffer> pBuffer);
		virtual ~IndirectArgsBufferDX11();

		virtual ResourceType				GetType();

	protected:

		friend RendererDX11;
	};
};
