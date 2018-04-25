#pragma once

namespace Apollo
{
	class DXGIOutput;

	class DXGIAdapter
	{
	public:
		DXGIAdapter(Microsoft::WRL::ComPtr<IDXGIAdapter1> pAdapter);
		virtual ~DXGIAdapter();

	protected:
		Microsoft::WRL::ComPtr<IDXGIAdapter1>		m_pAdapter;
		std::vector<DXGIOutput>						m_vOutputs;

		friend class RendererDX11;
	};
};
