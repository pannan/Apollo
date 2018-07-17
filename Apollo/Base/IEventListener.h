#pragma once
#include "KeyCodes.h"

namespace Apollo
{
	struct MouseEventArg
	{
		bool		lButton;
		bool		rButton;
		bool		mButton;
		bool		shiftButton;
		bool		controlButton;
		int		mouseX;
		int		mouseY;
	};

	class IEventListener
	{
	public:

		IEventListener(){}

		virtual ~IEventListener(){}

		virtual void onMouseMoveEvent(MouseEventArg* arg) = 0;

		virtual void	onKeyDownEvent(KeyCode code) = 0;

	protected:
	private:
	};
}
