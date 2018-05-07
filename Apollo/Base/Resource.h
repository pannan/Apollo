#pragma once

namespace Apollo
{

	enum RESOURCE_TYPE
	{
		//RT_NONE,//
		RT_HLSL, // = 0
		RT_MATERIAL,
		RT_TEXTURE
	};

	class Resource
	{
	public:

		Resource(const std::string& path,uint32_t handle) : m_path(path), m_handle(handle)
		{
		}

		virtual ~Resource(){}

	protected:

		uint32_t			m_handle;	//0-7 bit resourceid  8-31 resource index
		std::string			m_path;

	private:
	};
}
