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

	class RenderSkyOnCPU
	{
	public:

		RenderSkyOnCPU(Camera* camera,int w,int h);

		void		renderSingleScatting();

	protected:

		Vector3 uvToCameraRay(Vector2 inUV, const Matrix4x4& projMat, const Matrix4x4& inverseViewMat);

	protected:

		int		m_windowWidth;
		int		m_windowHeight;

		Camera*	m_camera;
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

		void		initTestTerrain();

		void		drawTestTerrain();

		void		test();

	private:

		Camera*					m_camera;

		ConstantBufferDX11Ptr m_globalParametersBuffer;
		ConstantBufferDX11Ptr	m_atmosphereParametersBuffer;

		ModelDX11Ptr				m_quadModelPtr;

		ModelDX11					m_testTerrainModel;

		ConstantBufferDX11Ptr	m_mvpMatrixBufferPtr;

		ShaderDX11Ptr			m_vsShader;
		ShaderDX11Ptr			m_psShader;

		RenderStateDX11		m_renderState;

		RenderStateDX11		m_testTerrainRenderState;

		AtmosphereParameters	m_atmosphereParameters;

		GlobalParameters			m_globalParameters;

		RenderSkyOnCPU*			m_renderSkyOnCPU;
	};
}
