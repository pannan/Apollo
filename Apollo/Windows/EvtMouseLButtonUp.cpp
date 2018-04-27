#include "stdafx.h"
#include "EvtMouseLButtonUp.h"
//--------------------------------------------------------------------------------
using namespace Apollo;
//--------------------------------------------------------------------------------
EvtMouseLButtonUp::EvtMouseLButtonUp(HWND hwnd, unsigned int wparam, long lparam)
	: EvtMouseMsg(hwnd, wparam, lparam)
{
}
//--------------------------------------------------------------------------------
EvtMouseLButtonUp::~EvtMouseLButtonUp()
{
}
//--------------------------------------------------------------------------------
std::wstring EvtMouseLButtonUp::GetEventName()
{
	return(std::wstring(L"lbutton_up"));
}
//--------------------------------------------------------------------------------
eEVENT EvtMouseLButtonUp::GetEventType()
{
	return(SYSTEM_LBUTTON_UP);
}