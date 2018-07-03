#pragma once
#include "MeshDX11.h"
#include "ShaderDX11.h"
#include "ConstantBufferDX11.h"
#include "Matrix4x4.h"
#include "Graphics/Camera.h"
namespace Apollo
{
	class Texture2dDX11;
	class HeightMapTerrain
	{
	public:

		HeightMapTerrain();
		~HeightMapTerrain();

		void  init();

		void	render();

	protected:

		void		createMesh();

		void		createShader();

	private:

		int		m_terrainSize;

		MeshDX11Ptr		m_terrainMesh;

		ShaderDX11Ptr		m_vsShader;
		ShaderDX11Ptr		m_psShader;

		Matrix4x4				m_mvpMat;
		ConstantBufferDX11Ptr m_mvpBuffer;

		Camera*					m_camera;

		Texture2dDX11*		m_heightMapTex;
	};
}
