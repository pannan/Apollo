#pragma once

#include "Resource.h"

namespace Apollo
{

	enum TEXTURE_TYPE
	{
		TextureType_1D,
		TextureType_2D,
		TextureType_3D,
		TextureType_CubeTex
	};

	class TextureResource : public Resource
	{
	public:

		TextureResource(const std::string& path, uint32_t handle, TEXTURE_TYPE type) : Resource(path, handle),m_type(type)
		{

		}

		virtual ~TextureResource(){}

		inline TEXTURE_TYPE getTextureType() { return m_type; }

	protected:

		TEXTURE_TYPE		m_type;

	private:

		TextureResource();
	};
}