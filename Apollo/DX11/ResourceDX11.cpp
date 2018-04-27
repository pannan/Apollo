#include "stdafx.h"
#include "ResourceDX11.h"
//--------------------------------------------------------------------------------
using namespace Apollo;
//--------------------------------------------------------------------------------
unsigned short ResourceDX11::s_usResourceUID = 0;
//--------------------------------------------------------------------------------
ResourceDX11::ResourceDX11()
{
	m_usInnerID = s_usResourceUID;
	s_usResourceUID++;
}
//--------------------------------------------------------------------------------
ResourceDX11::~ResourceDX11()
{
}
//--------------------------------------------------------------------------------
unsigned short ResourceDX11::GetInnerID()
{
	return(m_usInnerID);
}