#pragma once

#include "RendererDX11.h"

namespace Apollo
{
	class ResourceDX11
	{
	public:
		ResourceDX11();

		virtual ~ResourceDX11();

		virtual ResourceType				GetType() = 0;
		virtual ID3D11Resource*				GetResource() = 0;

		virtual UINT						GetEvictionPriority() = 0;
		virtual void						SetEvictionPriority(UINT EvictionPriority) = 0;

		unsigned short						GetInnerID();

		static unsigned short				s_usResourceUID;
		unsigned short						m_usInnerID;
	};
}