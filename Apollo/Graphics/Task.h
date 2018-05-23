#pragma once

namespace Apollo
{
	class Task
	{
	public:
		Task();
		virtual ~Task();

		virtual void update(float fTime) = 0;

		virtual void executeTask() = 0;

		virtual std::string getName() = 0;
	};
}
