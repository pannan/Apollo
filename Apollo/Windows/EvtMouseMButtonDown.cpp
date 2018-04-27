#include "stdafx.h"
#include "EvtMouseMButtonDown.h"
//--------------------------------------------------------------------------------
using namespace Apollo;
//--------------------------------------------------------------------------------
EvtMouseMButtonDown::EvtMouseMButtonDown(HWND hwnd, unsigned int wparam, long lparam)
	: EvtMouseMsg(hwnd, wparam, lparam)
{
}
//--------------------------------------------------------------------------------
EvtMouseMButtonDown::~EvtMouseMButtonDown()
{
}
//--------------------------------------------------------------------------------
std::wstring EvtMouseMButtonDown::GetEventName()
{
	return(std::wstring(L"mbutton_down"));
}
//--------------------------------------------------------------------------------
eEVENT EvtMouseMButtonDown::GetEventType()
{
	return(SYSTEM_MBUTTON_DOWN);
}