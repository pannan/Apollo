#pragma once

#include "BufferDX11.h"
//--------------------------------------------------------------------------------
namespace Apollo
{
	class StructuredBufferDX11 : public BufferDX11
	{
	public:
		StructuredBufferDX11(Microsoft::WRL::ComPtr<ID3D11Buffer> pBuffer);
		virtual ~StructuredBufferDX11();

		virtual ResourceType				GetType();

	protected:

		friend RendererDX11;
	};
};
