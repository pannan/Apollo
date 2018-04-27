#include "stdafx.h"
#include "EvtMouseRButtonUp.h"
//--------------------------------------------------------------------------------
using namespace Apollo;
//--------------------------------------------------------------------------------
EvtMouseRButtonUp::EvtMouseRButtonUp(HWND hwnd, unsigned int wparam, long lparam)
	: EvtMouseMsg(hwnd, wparam, lparam)
{
}
//--------------------------------------------------------------------------------
EvtMouseRButtonUp::~EvtMouseRButtonUp()
{
}
//--------------------------------------------------------------------------------
std::wstring EvtMouseRButtonUp::GetEventName()
{
	return(std::wstring(L"rbutton_up"));
}
//--------------------------------------------------------------------------------
eEVENT EvtMouseRButtonUp::GetEventType()
{
	return(SYSTEM_RBUTTON_UP);
}