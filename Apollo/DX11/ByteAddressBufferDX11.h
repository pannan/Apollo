#pragma once

#include "BufferDX11.h"
//--------------------------------------------------------------------------------
namespace Apollo
{
	class ByteAddressBufferDX11 : public BufferDX11
	{
	public:
		ByteAddressBufferDX11(Microsoft::WRL::ComPtr<ID3D11Buffer> pBuffer);
		virtual ~ByteAddressBufferDX11();

		virtual ResourceType				GetType();

	protected:

		friend RendererDX11;
	};
};
