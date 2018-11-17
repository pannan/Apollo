#pragma once

#include "RenderPass.h"

namespace Apollo
{
	class ScreenSpaceRenderPass : public RenderPass
	{
	public:

		ScreenSpaceRenderPass();

		~ScreenSpaceRenderPass();

		virtual void update(float fTime){}

		virtual void execute(){}

		virtual std::string getName() { return m_name; }

		virtual void setName(std::string name) { m_name = name; }

	protected:

		std::string  m_name;

	private:
	};
}
