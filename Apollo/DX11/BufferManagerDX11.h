#pragma once

#include "Singleton.h"

namespace Apollo
{

	//typedef Microsoft::WRL::ComPtr<ID3D11Buffer> BufferComPtr;
#define  BufferComPtr Microsoft::WRL::ComPtr<ID3D11Buffer>

	class BufferManagerDX11 : public SingletonEx<BufferManagerDX11>
	{
	public:

		BufferManagerDX11();

		~BufferManagerDX11();


		BufferComPtr			createBuffer(D3D11_BUFFER_DESC* pConfig, D3D11_SUBRESOURCE_DATA* pData);

	protected:

		std::vector<BufferComPtr>		m_bufferList;
	private:
	};
}
