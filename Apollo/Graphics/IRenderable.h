#pragma once

#include "MaterialDX11.h"
#include "AABB.h"

namespace Apollo
{
	class IRenderable
	{
	public:

		IRenderable() : m_handle(0), m_materialID(0){}

		virtual ~IRenderable(){}

		virtual void render() = 0;

		void		setMaterialID(uint16_t id);

		uint16_t	getMaterialID() { return m_materialID; }

		bool operator==(const IRenderable& renderalbe)
		{
			return renderalbe.m_handle == m_handle;
		}

		bool operator<(const IRenderable& renderalbe)
		{
			return m_handle < renderalbe.m_handle;
		}

		uint16_t			m_materialID;
		uint32_t			m_handle;// 0-15 material id

		AABB				m_aabb;

	protected:
	private:
	};
}
