#pragma once

#include "SampleBase.h"
#include "ConstantBufferDX11.h"
#include "ModelDX11.h"
#include "ShaderDX11.h"
#include "RenderStateDX11.h"

namespace Apollo
{
	class Camera;

	struct  GlobalParameters
	{
		Vector4		eyeWorldSpacePosition;
		Vector4		eyeEarthSpacePosition;
		//float4x4	inverseViewMatrix;
		//float4x4	inverseProjMatrix;
		Matrix4x4 inverseViewProjMatrix;
		Matrix4x4	inverseViewMatrix;
		Vector4	projMat[4];
	};

	struct AtmosphereParameters
	{
		float  top_radius;
		float  bottom_radius;
		Vector2	expand;
	};

	class SkySample : public SampleBase
	{
	public:
		SkySample();
		~SkySample();

		virtual void  init();

		virtual void	render();

		virtual Camera* getCamera() { return m_camera; }

	protected:

		void		initQuadMesh();

		void		test();

	private:

		Camera*					m_camera;

		ConstantBufferDX11Ptr m_globalParametersBuffer;
		ConstantBufferDX11Ptr	m_atmosphereParametersBuffer;

		ModelDX11Ptr				m_quadModelPtr;

		ShaderDX11Ptr			m_vsShader;
		ShaderDX11Ptr			m_psShader;

		RenderStateDX11		m_renderState;

		AtmosphereParameters	m_atmosphereParameters;

		GlobalParameters			m_globalParameters;
	};
}
