#include "stdafx.h"
#include "DXGIAdapter.h"
#include "DXGIOutput.h"

using namespace Apollo;

DXGIAdapter::DXGIAdapter(Microsoft::WRL::ComPtr<IDXGIAdapter1> pAdapter)
{
	m_pAdapter = pAdapter;

	Microsoft::WRL::ComPtr<IDXGIOutput> pOutput;

	while (pAdapter->EnumOutputs(m_vOutputs.size(), pOutput.ReleaseAndGetAddressOf()) != DXGI_ERROR_NOT_FOUND)
	{
		m_vOutputs.push_back(pOutput);
	}
}
//--------------------------------------------------------------------------------
DXGIAdapter::~DXGIAdapter()
{
}