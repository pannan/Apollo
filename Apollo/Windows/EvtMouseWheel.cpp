#include "stdafx.h"
#include "EvtMouseWheel.h"
//--------------------------------------------------------------------------------
using namespace Apollo;
//--------------------------------------------------------------------------------
EvtMouseWheel::EvtMouseWheel(HWND hwnd, unsigned int wparam, long lparam)
	: EvtMouseMsg(hwnd, wparam, lparam)
{
}
//--------------------------------------------------------------------------------
EvtMouseWheel::~EvtMouseWheel()
{
}
//--------------------------------------------------------------------------------
std::wstring EvtMouseWheel::GetEventName()
{
	return(std::wstring(L"mouse_wheel"));
}
//--------------------------------------------------------------------------------
eEVENT EvtMouseWheel::GetEventType()
{
	return(SYSTEM_MOUSE_WHEEL);
}
//--------------------------------------------------------------------------------
int EvtMouseWheel::GetWheelDelta()
{
	return((short)((0xffff0000 & m_wparam) >> 16));
}