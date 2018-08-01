#pragma once

#include "MaterialDX11.h"
#include "AABB.h"

namespace Apollo
{
	class IRenderable
	{
	public:

		IRenderable() : m_handle(0){}

		virtual ~IRenderable(){}

		void		setMaterialID(uint16_t id);

		bool operator==(const IRenderable& renderalbe)
		{
			return renderalbe.m_handle == m_handle;
		}

		bool operator<(const IRenderable& renderalbe)
		{
			return m_handle < renderalbe.m_handle;
		}

		uint32_t			m_handle;// 0-15 material id

		AABB				m_aabb;

	protected:
	private:
	};
}
