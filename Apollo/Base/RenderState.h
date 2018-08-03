#pragma once

namespace Apollo
{
	class RenderState
	{
	public:

		RenderState(){}
		virtual ~RenderState() = 0;

		virtual void bind() = 0;


		virtual void unBind() = 0;

	protected:
	private:
	};
}
