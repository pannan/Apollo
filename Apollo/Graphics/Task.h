#pragma once

namespace Apollo
{
	class Task
	{
	public:
		Task(){}
		virtual ~Task(){}

		virtual void update(float fTime) = 0;

		virtual void execute() = 0;

		virtual std::string getName() = 0;

		virtual void setName(std::string name) = 0;
	};
}
