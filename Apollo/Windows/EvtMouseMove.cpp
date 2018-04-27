#include "stdafx.h"
#include "EvtMouseMove.h"
//--------------------------------------------------------------------------------
using namespace Apollo;
//--------------------------------------------------------------------------------
EvtMouseMove::EvtMouseMove(HWND hwnd, unsigned int wparam, long lparam)
	: EvtMouseMsg(hwnd, wparam, lparam)
{
}
//--------------------------------------------------------------------------------
EvtMouseMove::~EvtMouseMove()
{
}
//--------------------------------------------------------------------------------
std::wstring EvtMouseMove::GetEventName()
{
	return(std::wstring(L"mouse_move"));
}
//--------------------------------------------------------------------------------
eEVENT EvtMouseMove::GetEventType()
{
	return(SYSTEM_MOUSE_MOVE);
}