#pragma once

namespace Apollo
{
	class Camera;

	class SampleBase
	{
	public:

		SampleBase(){}

		virtual ~SampleBase(){}

		virtual void init() = 0;

		virtual void render() = 0;

		virtual Camera* getCamera() = 0;

	protected:
	private:
	};
}
