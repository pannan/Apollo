#include "stdafx.h"
#include "MaterialDX11.h"

using namespace Apollo;

void MaterialDX11::bind() const
{
	if (m_vs)
		m_vs->bin();

	if (m_ps)
		m_ps->bin();
}

void MaterialDX11::unBind() const
{
	if (m_vs)
		m_vs->unBin();

	if (m_ps)
		m_ps->unBin();
}