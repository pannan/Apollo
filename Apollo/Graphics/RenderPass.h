#pragma once

#include "Task.h"

namespace Apollo
{
	class RenderPass : public Task
	{
	public:

		RenderPass();

		~RenderPass();

		virtual void update(float fTime);

		virtual void executeTask();

		virtual std::string getName() { return m_name; }

	protected:

		std::string  m_name;

	private:
	};
}
