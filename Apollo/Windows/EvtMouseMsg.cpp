#include "stdafx.h"
#include "EvtMouseMsg.h"
//--------------------------------------------------------------------------------
using namespace Apollo;
//--------------------------------------------------------------------------------
EvtMouseMsg::EvtMouseMsg(HWND hwnd, unsigned int wparam, long lparam)
	: EvtWindowMsg(hwnd, wparam, lparam)
{
}
//--------------------------------------------------------------------------------
EvtMouseMsg::~EvtMouseMsg()
{
}
//--------------------------------------------------------------------------------
bool EvtMouseMsg::ControlKeyDown()
{
	return((MK_CONTROL & m_wparam) > 0);
}
//--------------------------------------------------------------------------------
bool EvtMouseMsg::LButtonDown()
{
	return((MK_LBUTTON & m_wparam) > 0);
}
//--------------------------------------------------------------------------------
bool EvtMouseMsg::MButtonDown()
{
	return((MK_MBUTTON & m_wparam) > 0);
}
//--------------------------------------------------------------------------------
bool EvtMouseMsg::RButtonDown()
{
	return((MK_RBUTTON & m_wparam) > 0);
}
//--------------------------------------------------------------------------------
bool EvtMouseMsg::ShiftKeyDown()
{
	return((MK_SHIFT & m_wparam) > 0);
}
//--------------------------------------------------------------------------------
int EvtMouseMsg::GetX()
{
	return(0xffff & m_lparam);
}
//--------------------------------------------------------------------------------
int EvtMouseMsg::GetY()
{
	return((0xffff0000 & m_lparam) >> 16);
}
//---------------------------------