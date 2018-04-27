#pragma once

#include "EvtWindowMsg.h"

namespace Apollo
{
	class EvtKeyboardMsg : public EvtWindowMsg
	{
	public:
		EvtKeyboardMsg(HWND hwnd, unsigned int wparam, long lparam);
		virtual ~EvtKeyboardMsg();

		unsigned int GetCharacterCode();
		unsigned int GetRepeatCount();
		unsigned int GetScanCode();
		bool GetExtendedKey();
		bool GetContextCode();
		bool GetPreviousKeyState();
		bool GetTransitionState();
	};

	typedef std::shared_ptr<EvtKeyboardMsg> EvtKeyboardMsgPtr;
}
