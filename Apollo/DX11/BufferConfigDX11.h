#pragma once

#include "RendererDX11.h"
//--------------------------------------------------------------------------------
namespace Apollo
{
	class BufferConfigDX11
	{
	public:
		BufferConfigDX11();
		virtual ~BufferConfigDX11();

		void SetDefaults();

		void SetDefaultConstantBuffer(UINT size, bool dynamic);
		void SetDefaultVertexBuffer(UINT size, bool dynamic);
		void SetDefaultIndexBuffer(UINT size, bool dynamic);
		void SetDefaultStructuredBuffer(UINT size, UINT structsize);
		void SetDefaultByteAddressBuffer(UINT size);
		void SetDefaultIndirectArgsBuffer(UINT size);
		void SetDefaultStagingBuffer(UINT size);

		void SetByteWidth(UINT state);
		void SetUsage(D3D11_USAGE state);
		void SetBindFlags(UINT state);
		void SetCPUAccessFlags(UINT state);
		void SetMiscFlags(UINT state);
		void SetStructureByteStride(UINT state);

		D3D11_BUFFER_DESC GetBufferDesc();

	protected:
		D3D11_BUFFER_DESC 		m_State;

		friend RendererDX11;
	};
};