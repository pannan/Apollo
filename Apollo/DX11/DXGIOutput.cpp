#include "stdafx.h"
#include "DXGIOutput.h"

using namespace Apollo;

DXGIOutput::DXGIOutput(Microsoft::WRL::ComPtr<IDXGIOutput> pOutput)
{
	m_pOutput = pOutput;
}
//--------------------------------------------------------------------------------
DXGIOutput::~DXGIOutput()
{
}