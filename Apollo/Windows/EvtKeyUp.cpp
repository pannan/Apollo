#include "stdafx.h"
#include "EvtKeyUp.h"
//--------------------------------------------------------------------------------
using namespace Apollo;
//--------------------------------------------------------------------------------
EvtKeyUp::EvtKeyUp(HWND hwnd, unsigned int wparam, long lparam)
	: EvtKeyboardMsg(hwnd, wparam, lparam)
{
}
//--------------------------------------------------------------------------------
EvtKeyUp::~EvtKeyUp()
{
}
//--------------------------------------------------------------------------------
std::wstring EvtKeyUp::GetEventName()
{
	return(std::wstring(L"keyboard_keyup"));
}
//--------------------------------------------------------------------------------
eEVENT EvtKeyUp::GetEventType()
{
	return(SYSTEM_KEYBOARD_KEYUP);
}