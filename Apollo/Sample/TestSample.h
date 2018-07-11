#pragma once

#include "ShaderDX11.h"
#include "ConstantBufferDX11.h"
#include "RenderStateDX11.h"
#include "Vector2f.h"
#include "Vector3f.h"
#include "MeshDX11.h"

namespace Apollo
{
	//���sample��Ϊ�˲�����cs���ȡstructbuffer
	class TestSample
	{
	public:

		void  init();

		void render();

	protected:

		void initQuadMesh();

	private:

		int						m_terrainSize;

	
		ShaderDX11Ptr		m_vsShader;
		ShaderDX11Ptr		m_psShader;

		ShaderDX11Ptr		m_computerFetchColorToTextureShader;
		

		RenderStateDX11	m_renderState;

		StructuredBufferDX11Ptr	m_ColorStructBuffer;

		MeshDX11Ptr m_quadMesh;
	};
}
