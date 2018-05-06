#pragma once

namespace Apollo
{
	enum ResourceType
	{
		RT_VERTEXBUFFER = 0x010000,
		RT_INDEXBUFFER = 0x020000,
		RT_CONSTANTBUFFER = 0x030000,
		RT_STRUCTUREDBUFFER = 0x040000,
		RT_BYTEADDRESSBUFFER = 0x050000,
		RT_INDIRECTARGSBUFFER = 0x060000,
		RT_TEXTURE1D = 0x070000,
		RT_TEXTURE2D = 0x080000,
		RT_TEXTURE3D = 0x090000
	};

	class IResourceDX11
	{
	public:

		IResourceDX11();

		virtual ~IResourceDX11();

		virtual ResourceType						GetType() = 0;
		virtual ID3D11Resource*				GetResource() = 0;

		virtual UINT						GetEvictionPriority() = 0;
		virtual void						SetEvictionPriority(UINT EvictionPriority) = 0;

		unsigned short						GetInnerID();

		static unsigned short				s_usResourceUID;
		unsigned short						m_usInnerID;

	protected:
	private:
	};
}
