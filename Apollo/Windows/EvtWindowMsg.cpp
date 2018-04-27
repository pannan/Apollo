#include "stdafx.h"
#include "EvtWindowMsg.h"
//--------------------------------------------------------------------------------
using namespace Apollo;
//--------------------------------------------------------------------------------
EvtWindowMsg::EvtWindowMsg(HWND hwnd, unsigned int wparam, long lparam)
{
	m_hWnd = hwnd;
	m_wparam = wparam;
	m_lparam = lparam;
}
//--------------------------------------------------------------------------------
EvtWindowMsg::~EvtWindowMsg()
{
}
//--------------------------------------------------------------------------------
HWND EvtWindowMsg::GetWindowHandle()
{
	return(m_hWnd);
}
//--------------------------------------------------------------------------------
unsigned int EvtWindowMsg::GetWParam()
{
	return(m_wparam);
}
//--------------------------------------------------------------------------------
unsigned int EvtWindowMsg::GetLParam()
{
	return(m_lparam);
}