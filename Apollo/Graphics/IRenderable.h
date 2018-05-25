#pragma once

#include "MaterialDX11.h"

namespace Apollo
{
	class IRenderable
	{
	public:

		IRenderable();

		virtual ~IRenderable();


		MaterialDX11Ptr		m_mat;

	protected:
	private:
	};
}
