#pragma once

namespace Apollo
{
	class MaterialDX11
	{
	public:

		MaterialDX11();

		virtual ~MaterialDX11();

//	protected:

		uint32_t	m_vs;
		uint32_t	m_ps;

	private:
	};

	typedef std::shared_ptr<MaterialDX11> MaterialDX11Ptr;
}
