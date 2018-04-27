#include "stdafx.h"
#include "EvtMouseMButtonUp.h"
//--------------------------------------------------------------------------------
using namespace Apollo;
//--------------------------------------------------------------------------------
EvtMouseMButtonUp::EvtMouseMButtonUp(HWND hwnd, unsigned int wparam, long lparam)
	: EvtMouseMsg(hwnd, wparam, lparam)
{
}
//--------------------------------------------------------------------------------
EvtMouseMButtonUp::~EvtMouseMButtonUp()
{
}
//--------------------------------------------------------------------------------
std::wstring EvtMouseMButtonUp::GetEventName()
{
	return(std::wstring(L"mbutton_up"));
}
//--------------------------------------------------------------------------------
eEVENT EvtMouseMButtonUp::GetEventType()
{
	return(SYSTEM_MBUTTON_UP);
}