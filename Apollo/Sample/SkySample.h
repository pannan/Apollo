#pragma once

#include "SampleBase.h"
#include "ConstantBufferDX11.h"

namespace Apollo
{
	class SkySample : public SampleBase
	{
	public:
		SkySample();
		~SkySample();

		virtual void  init();

		virtual void	render();

		virtual Camera* getCamera() { return m_camera; }

	private:

		Camera*					m_camera;

		ConstantBufferDX11Ptr m_matrixBuffer;
}
