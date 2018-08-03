#include "stdafx.h"
#include "RenderTargetDX11.h"
#include "RendererDX11.h"
#include "Texture2dDX11.h"
using namespace Apollo;

RenderTargetDX11::RenderTargetDX11()
{
	m_width = 0;
	m_height = 0;

	m_device = RendererDX11::getInstance().getDevice();
	m_deviceContext = RendererDX11::getInstance().getDeviceContex();

	m_textureList.resize((size_t)AttachmentPoint::NumAttachmentPoints);
	//m_structuredBufferList.resize(8);

	for (size_t i = 0; i < (size_t)AttachmentPoint::NumAttachmentPoints; ++i)
		m_textureList[i] = nullptr;

	//for (size_t i = 0; i < 8; ++i)
	//	m_structuredBufferList[i] = nullptr;
}

RenderTargetDX11::~RenderTargetDX11()
{

}

void RenderTargetDX11::attachTexture(AttachmentPoint attachment, Texture2dDX11* tex)
{
	m_textureList[(uint8_t)attachment] = tex;
}

Texture2dDX11* RenderTargetDX11::getTexture(AttachmentPoint attachment)
{
	return m_textureList[(uint8_t)attachment];
}

void RenderTargetDX11::clear(AttachmentPoint attachment, ClearFlags clearFlags, const Vector4& color,float depth, uint8_t stencil)
{
	Texture2dDX11* tex = m_textureList[(uint8_t)attachment];
	if (tex)
		tex->clear(clearFlags, color, depth, stencil);
}

void RenderTargetDX11::clear(ClearFlags clearFlags /* = ClearFlags::All */, const Vector4& color /* = Vector4::ZERO */, float depth /* = 1.0f */, uint8_t stencil /* = 0 */)
{
	for (uint8_t i = 0; i < (uint8_t)AttachmentPoint::NumAttachmentPoints; ++i)
	{
		clear((AttachmentPoint)i,clearFlags,color,depth,stencil);
	}
}

//void RenderTargetDX11::attachStructuredBuffer(uint8_t slot,StructuredBufferDX11* structuredBuffer)
//{
//	m_structuredBufferList[slot] = structuredBuffer;
//}

void RenderTargetDX11::resize(int width, int height)
{

}

void RenderTargetDX11::bind()
{
	ID3D11RenderTargetView* renderTargetViews[8];
	UINT numRTVs = 0;

	for (uint8_t i = 0; i < 8; i++)
	{
		Texture2dDX11* tex = m_textureList[i];
		if (tex)
		{
			renderTargetViews[numRTVs++] = tex->getRendertargetView();
		}
	}

	/*ID3D11UnorderedAccessView* uavViews[8];
	UINT uavStartSlot = numRTVs;
	UINT numUAVs = 0;

	for (uint8_t i = 0; i < 8; i++)
	{
		StructuredBufferDX11* rwBuffer = m_structuredBufferList[i];
		if (rwBuffer)
		{
			uavViews[numUAVs++] = rwbuffer->GetUnorderedAccessView();
		}
	}*/

	ID3D11DepthStencilView* depthStencilView = nullptr;
	Texture2dDX11* depthTexture = m_textureList[(uint8_t)AttachmentPoint::Depth];
	Texture2dDX11* depthStencilTexture = m_textureList[(uint8_t)AttachmentPoint::DepthStencil];

	if (depthTexture)
	{
		depthStencilView = depthTexture->getDepthStencilView();
	}
	else if (depthStencilTexture)
	{
		depthStencilView = depthStencilTexture->getDepthStencilView();
	}

	//m_deviceContext->OMSetRenderTargetsAndUnorderedAccessViews(numRTVs, renderTargetViews, depthStencilView, uavStartSlot, numUAVs, uavViews, nullptr);
	m_deviceContext->OMSetRenderTargets(numRTVs, renderTargetViews, depthStencilView);
}

void RenderTargetDX11::unBind()
{

}