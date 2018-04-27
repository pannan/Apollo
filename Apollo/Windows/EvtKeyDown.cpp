#include "stdafx.h"
#include "EvtKeyDown.h"
//--------------------------------------------------------------------------------
using namespace Apollo;
//--------------------------------------------------------------------------------
EvtKeyDown::EvtKeyDown(HWND hwnd, unsigned int wparam, long lparam)
	: EvtKeyboardMsg(hwnd, wparam, lparam)
{
}
//--------------------------------------------------------------------------------
EvtKeyDown::~EvtKeyDown()
{
}
//--------------------------------------------------------------------------------
std::wstring EvtKeyDown::GetEventName()
{
	return(std::wstring(L"keyboard_keydown"));
}
//--------------------------------------------------------------------------------
eEVENT EvtKeyDown::GetEventType()
{
	return(SYSTEM_KEYBOARD_KEYDOWN);
}