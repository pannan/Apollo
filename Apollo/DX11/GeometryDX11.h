#pragma once

#include "BufferManagerDX11.h"

namespace Apollo
{
	class GeometryDX11
	{
	public:

		//GeometryDX11();

		//~GeometryDX11();

		void		createFullSrceenQuad();

	protected:

		BufferComPtr		m_indexBuffer;
		BufferComPtr		m_vertexBuffer;

	private:
	};
}
